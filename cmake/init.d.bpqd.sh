#! /bin/sh
### BEGIN INIT INFO
# Provides:          bpqd
# Required-Start:    $all
# Required-Stop:
# Default-Start:     2 3 4 5
# Default-Stop:
# Short-Description: BPQ server
### END INIT INFO

user="kfg";

dir="/home/${user}";
daemon="bpqd";
pidfile="${dir}/.bpq/${daemon}.pid";
daemonfull="${dir}/bpq161/src/bpqd";
# args="-daemon -addnode=${peer}";
args="-daemon";

do_start ()
{
   echo "Starting BPQ server..."
   echo "start-stop-daemon --start --chuid $user --chdir $dir --pidfile $pidfile --startas $daemonfull"
   # start-stop-daemon --start --chuid $user --chdir $dir --pidfile $pidfile --startas $daemonfull -- $args
   start-stop-daemon --start --chuid $user --chdir $dir --pidfile $pidfile --startas $daemonfull -- $args
}

do_stop ()
{
   echo "Stopping BPQ server..."
   echo "start-stop-daemon --stop --chuid $user --chdir $dir --pidfile $pidfile --startas $daemonfull"
   start-stop-daemon --stop --chuid $user --chdir $dir --pidfile $pidfile --startas $daemonfull
}

do_status ()
{
   echo "Checking status of BPQ server..."
   echo "start-stop-daemon --status --chuid $user --chdir $dir --pidfile $pidfile --startas $daemonfull"
   start-stop-daemon --status --chuid $user --chdir $dir --pidfile $pidfile --startas $daemonfull
}

case "$1" in
  start|"")
   do_start
   ;;
  stop)
   do_stop
   ;;
  status)
   do_status
   ;;
  restart)
   do_stop
   sleep 10
   do_start
   ;;
  *)
   echo "Wrong arguments"
   exit 3
   ;;
esac