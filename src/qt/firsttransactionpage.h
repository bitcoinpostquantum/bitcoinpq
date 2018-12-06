// Copyright (c) 2018 The Bitcoin Post-Quantum developers

#ifndef _BPQ_FIRSTTRANSACTIONPAGE_H
#define _BPQ_FIRSTTRANSACTIONPAGE_H

#include <qt/guiutil.h>
#include <qt/bitcoinunits.h>
#include <qt/walletmodel.h>

#include <QDialog>
#include <QAbstractTableModel>
#include <memory>


class PlatformStyle;
class WalletModel;
class CKey;

namespace Ui {
    class FirstTransactionPage;
}

class FirstTransactionPage : public QDialog 
{
	Q_OBJECT
public:
	
    explicit FirstTransactionPage(const PlatformStyle *platformStyle, QWidget *parent = 0);
	virtual ~FirstTransactionPage();
	
	void setModel(WalletModel *_model);
	
	
public Q_SLOTS:
	
    void clear();
    void reject();
    void accept();
	void updateTabsAndLabels();
    void setBitcoinBalance(const CAmount& balance);
    void setBalance(const CAmount& balance, const CAmount& unconfirmedBalance, const CAmount& immatureBalance,
                    const CAmount& watchOnlyBalance, const CAmount& watchUnconfBalance, const CAmount& watchImmatureBalance);
	
private Q_SLOTS:

    void on_buttonImportKey_clicked();
    void on_buttonRescan_clicked();
    void rescan_run();
    void rescan_finished();
    void on_buttonSendToBpq_clicked();

    void coinControlButtonClicked();
	void coinControlUpdateLabels();
	
 Q_SIGNALS:
    // Fired when a message should be reported to the user
    void message(const QString &title, const QString &message, unsigned int style);
	
protected:
	
	void importKey(CKey const & key);
	
	void resizeEvent(QResizeEvent* event) override;

    void updateBalance();
	
	void updateCoinControlState(CCoinControl& ctrl);
    // Process WalletModel::SendCoinsReturn and generate a pair consisting
    // of a message and message flags for use in Q_EMIT message().
    // Additional parameter msgArg can be used via .arg(msgArg).
	void processSendCoinsReturn(const WalletModel::SendCoinsReturn &sendCoinsReturn, const QString &msgArg = QString());
	
private:
	Ui::FirstTransactionPage *ui;
    const PlatformStyle *platformStyle;
    WalletModel *model;
	
	bool fNewRecipientAllowed; // TODO: remove or rename
	
	CAmount bitcoinsTotal;
	CAmount bitcoinsSelected;
        
    std::unique_ptr<QThread> m_rescan_thread;
};

#endif /* _BPQ_FIRSTTRANSACTIONPAGE_H */
