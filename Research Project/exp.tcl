# This program has 4 arguments
# - Bw [bandwith of the bottleneck]
# - Tp [propagation time]
# - D  [delay]
# - N  [number of senders and receivers]

if {$argc != 4} {
  puts "$argc : Incorrect number of arguments"
  puts "usage:  ns essais.tcl Bw(kbps) Tp(ms) D(ms) N"
  exit 0
}

set Bw [lindex $argv  0]
append Bw Kb

set Tp [lindex $argv  1]
append Tp ms

set D  [lindex $argv  2]
#append D ms

set N  [lindex $argv  3]

puts "Bw = $Bw   Tp= $Tp    D = $D    N = $N"

set wnd 10000

#Create a simulator object
set ns [new Simulator]

#Open the nam trace file
set nf [open out.nam w]
$ns namtrace-all $nf

#Define a 'finish' procedure
proc finish {} {
        global ns nf cwnd_chan filetrace fqueue queuechan tcp0 tcp1 StartTime2 fmon fmonfile

        $ns flush-trace

  #Close out
        close $nf
        #close $cwnd_chan
        #close $queuechan
        #close $filetrace

        #$fmon dump
        #close $fmonfile

    # Collecting Throughput
      set tf   [open thgt.tr w]
      set time [$ns now]

      #set thgpt0 [$tcp0 set ack_]

    # assumes packet of 1000 bytes
      #set thgpt0 [expr $thgpt0 * 1000.0 * 8.0 / ($time - 0.200) ]

      #puts $tf "$thgpt0 $thgpt1"
      close $tf

  #Execute nam on the trace file
        #exec nam out.nam &
        exit 0
}


# Code space

#--------NODE CREATION--------#
#Create N senders
for {set i 1} {$i <= $N} {incr i} {
      set s($i) [$ns node]
}

#Create N receivers
for {set i 1} {$i <= $N} {incr i} {
      set r($i) [$ns node]
}

#Create router nodes (Sending Router R, Destination Router Rd)
set R  [$ns node] 
set Rd [$ns node]

#--------LINK CREATION--------#
set Step [expr (180 / $N) / 2]

#Create links between the sender nodes and sending router
for {set i 1} {$i <= $N} {incr i} {
        $ns duplex-link $s($i) $R $Bw [expr ($i -1)*($D)] DropTail
        $ns duplex-link-op $R $s($i) orient left
}

#Create links between the receiver nodes and destination router
for {set i 1} {$i <= $N} {incr i} {
        $ns duplex-link $r($i) $Rd $Bw [expr ($i -1)*($D)] DropTail
        $ns duplex-link-op $Rd $r($i) orient right 
}

$ns duplex-link $R $Rd $Bw $Tp DropTail
$ns duplex-link-op $Rd $R orient left

#Call the finish procedure after 5 seconds simulation time
$ns at 5.0 "finish"

#Run the simulation
$ns run