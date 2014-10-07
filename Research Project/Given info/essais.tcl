# I stop traffic for the sources src00
#-----------------------------------------------------------------------------
# essais.tcl
# Author: Saad Biaz.
# Dates: Nov 3, 2003.
# Notes: An ns template script containing multiple tools.
#
# s0   
#  \   
#   \r0______dst
#   /  
#  /   
# s1   
#-----------------------------------------------------------------------------


proc finish {} {
    global ns nf cwnd_chan filetrace fqueue queuechan tcp0 tcp1 StartTime2 fmon fmonfile

    $ns flush-trace
    close $nf
    close $cwnd_chan
    close $queuechan
    close $filetrace
    
    $fmon dump
    close $fmonfile

    # Collecting Throughput
    set tf   [open thgt.tr w]
    set time [$ns now]

    set thgpt0 [$tcp0 set ack_]
    # assumes packet of size 1000 bytes
    set thgpt0 [expr $thgpt0 * 1000.0 * 8.0 / ($time - 0.200) ]
    set thgpt1 [$tcp1 set ack_]
    set thgpt1 [expr $thgpt1 * 1000.0 * 8.0 / ($time - $StartTime2) ]
    puts $tf "$thgpt0 $thgpt1"
    close $tf

    # exec nam out.nam&
    exit 0
}

 if {$argc != 3} {
  puts "$argc : Incorrect number of arguments"
  puts "usage:  ns essais.tcl Bw(kbps) Tp(ms)  StartTime2(ms)"
  exit 0
}

set Bw          [lindex $argv  0]
append Bw Kb
set Tp          [lindex $argv  1]
append Tp ms
set StartTime2      [lindex $argv  2]
set StartTime2      [expr $StartTime2 / 1000.0];#convert into ms

puts "Bw = $Bw   Tp= $Tp     StartTime2 = $StartTime2"

set wnd 10000




#Create a new simulator object
set ns [new Simulator]

#Tcl statement, opens a file, get back a handle nf on this file
set nf [open out.nam w]


#Associates the file with nam tracing
$ns namtrace-all $nf



#Create my nodes
set s0  [$ns node]
set s1  [$ns node]
set r0  [$ns node]
set dst [$ns node]



#Create links
$ns duplex-link $s0 $r0 10Mb 2ms DropTail
$ns duplex-link $s1 $r0 10Mb 2ms DropTail

# This link has RED queue management only 
# for tracing purpose. 
# Because Min thresh= MaxThresh = queue limit
set queuelimit 50
Queue/RED set maxthresh_ $queuelimit
Queue/RED set thresh_ $queuelimit; # minthreshold
$ns duplex-link $r0 $dst $Bw $Tp RED



# Monitoring the queue
$ns duplex-link-op $r0 $dst queuePos 0.5

# Monitoring the queue

# Set a flow monitor
set r0dstlink [$ns link $r0 $dst]; # link to collect stats on
set fmon [$ns makeflowmon Fid]

$ns attach-fmon $r0dstlink $fmon

set fmonfile [open essaismon.tr w]

$fmon attach $fmonfile


#============ just for nam simulations ===============
#Arrange the layout 
$ns duplex-link-op $s0 $r0  orient right-down      
$ns duplex-link-op $s1 $r0  orient right-up 
$ns duplex-link-op $r0 $dst orient right

$ns color 0 Blue
$ns color 1 Red




# =============== TCP Connections =======================
#Create a TCP agent and attach it to node n0
set tcp0 [$ns create-connection TCP/Reno $s0 TCPSink $dst 0]
$tcp0 set window_ 10000
$tcp0 set class_ 0

set tcp1 [$ns create-connection TCP/Reno $s1 TCPSink $dst 0]
$tcp1 set window_ 10000
$tcp1 set class_ 1


# =============== Monitor Congestion window ===========
set cwnd_chan [open cwnd.tr w]
$tcp0 trace cwnd_
$tcp0 trace rtt_
$tcp0 attach $cwnd_chan 



# =============  Track drops          =============
set filetrace [open out.tr "w"]
$ns trace-queue $r0 $dst $filetrace


# ============= Monitor Queue length ================
set thequeue [[$ns link $r0 $dst] queue]
set queuechan [open queue.tr w]
$thequeue trace curq_
$thequeue attach $queuechan



# =============== FTP traffic  ========================
set ftp0 [$tcp0 attach-app FTP]
$ns at 0.200 "$ftp0 start"

set ftp1 [$tcp1 attach-app FTP]
$ns at $StartTime2 "$ftp1 start"



#Schecule  some events

$ns at 10s "finish"

$ns run








