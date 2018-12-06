// Copyright (c) 2018 The Bitcoin Post-Quantum developers

#include <qt/firsttransactionpage.h>

#include <qt/forms/ui_firsttransactionpage.h>

#include <qt/optionsmodel.h>
#include <qt/walletmodel.h>
#include <qt/bitcoinunits.h>
#include <qt/sendcoinsdialog.h>
#include <qt/coincontroldialog.h>

#include <QMessageBox>
#include <QThread>

#include "base58.h"
#include <wallet/wallet.h>
#include <wallet/coincontrol.h>
#include <validation.h> // mempool and minRelayTxFee

#include <uint256.h>
#include <script/standard.h>
#include <base58.h>

#include <util.h>

#include <qt/firsttransactionpage.moc>

FirstTransactionPage::FirstTransactionPage(const PlatformStyle *platformStyle, QWidget *parent)
: QDialog(parent)
, ui(new Ui::FirstTransactionPage)
, platformStyle(platformStyle)
, model(nullptr) {
    ui->setupUi(this);

    // Coin Control
    connect(ui->pushButtonCoinControl, SIGNAL(clicked()), this, SLOT(coinControlButtonClicked()));
    //connect(ui->checkBoxCoinControlChange, SIGNAL(stateChanged(int)), this, SLOT(coinControlChangeChecked(int)));
    //connect(ui->lineEditCoinControlChange, SIGNAL(textEdited(const QString &)), this, SLOT(coinControlChangeEdited(const QString &)));
}

FirstTransactionPage::~FirstTransactionPage() {
		
	if (m_rescan_thread)
	{
		m_rescan_thread->quit();
		m_rescan_thread->wait();
	}
	
    delete ui;
}

void FirstTransactionPage::setModel(WalletModel *_model) {
    this->model = _model;

    if (_model && _model->getOptionsModel()) {
        updateBalance();
        connect(_model, SIGNAL(bitcoinBalanceChanged(CAmount)), this, SLOT(setBitcoinBalance(CAmount)));
        connect(_model, SIGNAL(balanceChanged(CAmount, CAmount, CAmount, CAmount, CAmount, CAmount)), this, SLOT(setBalance(CAmount, CAmount, CAmount, CAmount, CAmount, CAmount)));

        coinControlUpdateLabels();
    }
}

void FirstTransactionPage::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
}

void FirstTransactionPage::on_buttonImportKey_clicked() {
    QString strKey = ui->editImportKey->text();
    ui->editImportKey->clear();

    if (strKey.isEmpty())
        return;

    CBitcoinSecret secret;
    secret.SetString(strKey.toStdString());

    if (!secret.IsBitcoinSecret()) {
        QMessageBox::critical(this, tr("Importing Failed"),
                tr("String is not valid Bitcoin private key."));
        return;
    }

    CKey key(secret.GetKey());

    if (!key.IsValid() || key.IsXMSS()) {
        QMessageBox::critical(this, tr("Importing Failed"),
                tr("String is not valid Bitcoin private key."));
        return;
    }

    importKey(secret.GetKey());
}

void FirstTransactionPage::on_buttonRescan_clicked() {
	if (!model || !model->getOptionsModel())
		return;
	
	if (m_rescan_thread)
		return;
	
	m_rescan_thread.reset(new QThread());
	
	connect(m_rescan_thread.get(), SIGNAL(started()), this, 
			SLOT(rescan_run()), Qt::DirectConnection);
	
	connect(m_rescan_thread.get(), SIGNAL(finished()), this, 
			SLOT(rescan_finished()), Qt::QueuedConnection);
	
	m_rescan_thread->start();
}

void FirstTransactionPage::rescan_run() 
{
	model->RescanBitcoins();
	m_rescan_thread->quit();
}

void FirstTransactionPage::rescan_finished() 
{
	m_rescan_thread.reset();
	updateBalance();
}

void FirstTransactionPage::on_buttonSendToBpq_clicked() {
    if (!model || !model->getOptionsModel())
        return;

    QString strAccount = "bpq";
    QString strAddress = ui->textAddressTo->text();
    if (strAddress.isEmpty())
        return;

    // Always use a CCoinControl instance, use the CoinControlDialog instance if CoinControl has been enabled
    CCoinControl ctrl = *CoinControlDialog::coinControl();

    CAmount nPayAmount = model->getBalance(&ctrl);

    if (!ctrl.HasSelected() || nPayAmount == 0) {
        return;
    }

    SendCoinsRecipient recipient;

    recipient.address = strAddress;
    //recipient.label = strAccount;
    recipient.amount = nPayAmount;
    recipient.fSubtractFeeFromAmount = true;

    QList<SendCoinsRecipient> recipients;
    recipients.append(recipient);

    fNewRecipientAllowed = false;

    WalletModel::UnlockContext ctx(model->requestUnlock());
    if (!ctx.isValid()) {
        // Unlock wallet was cancelled
        fNewRecipientAllowed = true;
        return;
    }

    // prepare transaction for getting txFee earlier
    WalletModelTransaction currentTransaction(recipients);
    WalletModel::SendCoinsReturn prepareStatus;

    updateCoinControlState(ctrl);

    prepareStatus = model->prepareTransaction(currentTransaction, ctrl);

    // process prepareStatus and on error generate message shown to user
    processSendCoinsReturn(prepareStatus,
            BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), currentTransaction.getTransactionFee()));

    if (prepareStatus.status != WalletModel::OK) {
        fNewRecipientAllowed = true;
        return;
    }

    CAmount txFee = currentTransaction.getTransactionFee();
    // Format confirmation message
    QStringList formatted;
    for (const SendCoinsRecipient &rcp : currentTransaction.getRecipients()) {
        // generate bold amount string
        QString amount = "<b>" + BitcoinUnits::formatHtmlWithUnit(model->getOptionsModel()->getDisplayUnit(), rcp.amount);
        amount.append("</b>");
        // generate monospace address string
        QString address = "<span style='font-family: monospace;'>" + rcp.address;
        address.append("</span>");

        QString recipientElement;

        if (!rcp.paymentRequest.IsInitialized()) // normal payment
        {
            if (rcp.label.length() > 0) // label with address
            {
                recipientElement = tr("%1 to %2").arg(amount, GUIUtil::HtmlEscape(rcp.label));
                recipientElement.append(QString(" (%1)").arg(address));
            } else // just address
            {
                recipientElement = tr("%1 to %2").arg(amount, address);
            }
        } else if (!rcp.authenticatedMerchant.isEmpty()) // authenticated payment request
        {
            recipientElement = tr("%1 to %2").arg(amount, GUIUtil::HtmlEscape(rcp.authenticatedMerchant));
        } else // unauthenticated payment request
        {
            recipientElement = tr("%1 to %2").arg(amount, address);
        }

        formatted.append(recipientElement);
    }

    QString questionString = tr("Are you sure you want to send?");
    questionString.append("<br /><br />%1");

    if (txFee > 0) {
        // append fee string if a fee is required
        questionString.append("<hr /><span style='color:#aa0000;'>");
        questionString.append(BitcoinUnits::formatHtmlWithUnit(model->getOptionsModel()->getDisplayUnit(), txFee));
        questionString.append("</span> ");
        questionString.append(tr("added as transaction fee"));

        // append transaction size
        questionString.append(" (" + QString::number((double) currentTransaction.getTransactionSize() / 1000) + " kB)");
    }

    // add total amount in all subdivision units
    questionString.append("<hr />");
    CAmount totalAmount = currentTransaction.getTotalTransactionAmount() + txFee;
    QStringList alternativeUnits;
    for (BitcoinUnits::Unit u : BitcoinUnits::availableUnits()) {
        if (u != model->getOptionsModel()->getDisplayUnit())
            alternativeUnits.append(BitcoinUnits::formatHtmlWithUnit(u, totalAmount));
    }
    questionString.append(tr("Total Amount %1")
            .arg(BitcoinUnits::formatHtmlWithUnit(model->getOptionsModel()->getDisplayUnit(), totalAmount)));
    questionString.append(QString("<span style='font-size:10pt;font-weight:normal;'><br />(=%1)</span>")
            .arg(alternativeUnits.join(" " + tr("or") + "<br />")));

    questionString.append("<hr /><span>");

    //if (ui->optInRBF->isChecked()) {
    questionString.append(tr("You can increase the fee later (signals Replace-By-Fee, BIP-125)."));
    //} else {
    //    questionString.append(tr("Not signalling Replace-By-Fee, BIP-125."));
    //}
    questionString.append("</span>");


    SendConfirmationDialog confirmationDialog(tr("Confirm send coins"),
            questionString.arg(formatted.join("<br />")), SEND_CONFIRM_DELAY, this);
    confirmationDialog.exec();
    QMessageBox::StandardButton retval = (QMessageBox::StandardButton)confirmationDialog.result();

    if (retval != QMessageBox::Yes) {
        fNewRecipientAllowed = true;
        return;
    }

    // now send the prepared transaction
    WalletModel::SendCoinsReturn sendStatus = model->sendCoins(currentTransaction);
    // process sendStatus and on error generate message shown to user
    processSendCoinsReturn(sendStatus);

    if (sendStatus.status == WalletModel::OK) {
        accept();
        CoinControlDialog::coinControl()->UnSelectAll();
        coinControlUpdateLabels();
    }

    fNewRecipientAllowed = true;
}

void FirstTransactionPage::clear() {
    updateTabsAndLabels();
}

void FirstTransactionPage::reject() {
    clear();
}

void FirstTransactionPage::accept() {
    clear();
}

void FirstTransactionPage::updateTabsAndLabels() {
    //setupTabChain(0);
    coinControlUpdateLabels();
}

void FirstTransactionPage::coinControlButtonClicked() {
    CoinControlDialog dlg(platformStyle, true /* fBitcoinOnly */);
    dlg.setModel(model);
    dlg.exec();
    coinControlUpdateLabels();
}

void FirstTransactionPage::updateCoinControlState(CCoinControl& ctrl) {
    //if (ui->radioCustomFee->isChecked()) {
    //    ctrl.m_feerate = CFeeRate(ui->customFee->value());
    //} else {
    ctrl.m_feerate.reset();
    //}

    // Avoid using global defaults when sending money from the GUI
    // Either custom fee will be used or if not selected, the confirmation target from dropdown box
    // TODO: 
    //ctrl.m_confirm_target = getConfTargetForIndex(ui->confTargetSelector->currentIndex());
    //ctrl.signalRbf = ui->optInRBF->isChecked();

    ctrl.signalRbf = true;
}

void FirstTransactionPage::processSendCoinsReturn(const WalletModel::SendCoinsReturn &sendCoinsReturn, const QString &msgArg) {
    QPair<QString, CClientUIInterface::MessageBoxFlags> msgParams;
    // Default to a warning message, override if error message is needed
    msgParams.second = CClientUIInterface::MSG_WARNING;

    // This comment is specific to SendCoinsDialog usage of WalletModel::SendCoinsReturn.
    // WalletModel::TransactionCommitFailed is used only in WalletModel::sendCoins()
    // all others are used only in WalletModel::prepareTransaction()
    switch (sendCoinsReturn.status) {
        case WalletModel::InvalidAddress:
            msgParams.first = tr("The recipient address is not valid. Please recheck.");
            break;
        case WalletModel::InvalidAmount:
            msgParams.first = tr("The amount to pay must be larger than 0.");
            break;
        case WalletModel::AmountExceedsBalance:
            msgParams.first = tr("The amount exceeds your balance.");
            break;
        case WalletModel::AmountWithFeeExceedsBalance:
            msgParams.first = tr("The total exceeds your balance when the %1 transaction fee is included.").arg(msgArg);
            break;
        case WalletModel::DuplicateAddress:
            msgParams.first = tr("Duplicate address found: addresses should only be used once each.");
            break;
        case WalletModel::TransactionCreationFailed:
            msgParams.first = tr("Transaction creation failed!");
            msgParams.second = CClientUIInterface::MSG_ERROR;
            break;
        case WalletModel::TransactionCommitFailed:
            msgParams.first = tr("The transaction was rejected with the following reason: %1").arg(sendCoinsReturn.reasonCommitFailed);
            msgParams.second = CClientUIInterface::MSG_ERROR;
            break;
        case WalletModel::AbsurdFee:
            msgParams.first = tr("A fee higher than %1 is considered an absurdly high fee.").arg(BitcoinUnits::formatWithUnit(model->getOptionsModel()->getDisplayUnit(), maxTxFee));
            break;
        case WalletModel::PaymentRequestExpired:
            msgParams.first = tr("Payment request expired.");
            msgParams.second = CClientUIInterface::MSG_ERROR;
            break;
            // included to prevent a compiler warning.
        case WalletModel::OK:
        default:
            return;
    }

    Q_EMIT message(tr("Send Coins"), msgParams.first, msgParams.second);
}

// Coin Control: update labels

void FirstTransactionPage::coinControlUpdateLabels() {
    if (!model || !model->getOptionsModel())
        return;

    updateCoinControlState(*CoinControlDialog::coinControl());

    // set pay amounts
    CoinControlDialog::payAmounts.clear();
    CoinControlDialog::fSubtractFeeFromAmount = true;

    /*
for(int i = 0; i < ui->entries->count(); ++i)
{
    SendCoinsEntry *entry = qobject_cast<SendCoinsEntry*>(ui->entries->itemAt(i)->widget());
    if(entry && !entry->isHidden())
    {
        SendCoinsRecipient rcp = entry->getValue();
        CoinControlDialog::payAmounts.append(rcp.amount);
        if (rcp.fSubtractFeeFromAmount)
                        CoinControlDialog::fSubtractFeeFromAmount = true;
    }
}
     */

    // actual coin control calculation
    CoinControlDialog::updateLabels(model, this);
}

void FirstTransactionPage::importKey(CKey const & key) {
    QString serror;
    if (!model->ImportBitcoinKey(key, "bitcoins", serror)) {
        QMessageBox::critical(this, tr("Importing Failed"),
                serror);
        return;
    }

    updateBalance();
}

void FirstTransactionPage::setBitcoinBalance(const CAmount& balance) {
    if (model && model->getOptionsModel()) {
        QString text = BitcoinUnits::format(0, balance) + QString(" BTC");
        ui->labelBalance->setText(text);
    }
}

void FirstTransactionPage::setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance,
        const CAmount& watchBalance, const CAmount& watchUnconfirmedBalance, const CAmount& watchImmatureBalance) {
    Q_UNUSED(unconfirmedBalance);
    Q_UNUSED(immatureBalance);
    Q_UNUSED(watchBalance);
    Q_UNUSED(watchUnconfirmedBalance);
    Q_UNUSED(watchImmatureBalance);

    updateBalance();
}

void FirstTransactionPage::updateBalance() {
    if (model) {
        CAmount balance = model->GetBitcoinBalance();
        setBitcoinBalance(balance);
    }
}
