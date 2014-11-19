#Define a 'finish' procedure
proc finish {} {
        global ns nf tcp filetrace StartTimeInterval N

        $ns flush-trace

  #Close out
        close $nf

    # Collecting Throughput
      set tf   [open thgt.tr w]
      set time [$ns now]

      set thgptResult ""

      for {set i 1} {$i <= $N} {incr i} {
        set thgpt($i) [$tcp($i) set ack_]
        set thgpt($i) [expr $thgpt($i) * 1000.0 * 8.0 / ($time - ($i * $StartTimeInterval)) ]
        set thgptResult [concat $thgptResult $thgpt($i)]
      }

    # assumes packet of 1000 bytes
      #set thgpt0 [expr $thgpt0 * 1000.0 * 8.0 / ($time - 0.200) ]

      puts $tf $thgptResult
      close $tf

  #Execute nam on the trace file
        #exec nam out.nam &
        exit 0
}

# This program has 6 arguments
# - Bw [bandwith of the bottleneck]
# - Tp [propagation time]
# - D  [delay]
# - N  [number of senders and receivers]

if {$argc != 5} {
  puts "$argc : Incorrect number of arguments"
  puts "usage:  ns exp.tcl Bw(kbps) Tp(ms) D(ms) N StartTimeInterval(ms)"
  exit 0
}

set Bw [lindex $argv  0]
append Bw Kb

set Tp [lindex $argv  1]
append Tp ms

set D  [lindex $argv  2]

set N  [lindex $argv  3]

set StartTimeInterval [lindex $argv 4]
set StartTimeInterval [expr $StartTimeInterval / 1000.0];

puts "Bw = $Bw   Tp= $Tp    D = $D    N = $N  Start Time Interval = $StartTimeInterval"

#Create a simulator object
set ns [new Simulator]

# Associated with network animator. Unnecessary for this project
#Open the nam trace file
set nf [open out.nam w]
$ns namtrace-all $nf

# Code space

# ------- NODE CREATION ------- #
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

# ------- LINK CREATION ------- #
set Step [expr (180 / $N) / 2]

#Create links between the sender nodes and sending router
for {set i 1} {$i <= $N} {incr i} {
        $ns duplex-link $s($i) $R $Bw [expr ($i -1)*($D)] DropTail
}

#Create links between the receiver nodes and destination router
for {set i 1} {$i <= $N} {incr i} {
        $ns duplex-link $r($i) $Rd $Bw [expr ($i -1)*($D)] DropTail
}

#Create link between routers
$ns duplex-link $R $Rd $Bw $Tp DropTail

# ------- TCP Connections ------- #
for {set i 1} {$i <= $N} {incr i} {
    set tcp($i) [$ns create-connection TCP/Newreno $s($i) TCPSink $r($i) 0]
    $tcp($i) set window_ 10000
}

# ------- FTP Traffic ------- #
for {set i 1} {$i <= $N} {incr i} {
    set ftp($i) [$tcp($i) attach-app FTP]
    $ns at [expr $i * $StartTimeInterval] "$ftp($i) start"
}

#Call the finish procedure after 5 seconds simulation time
$ns at 50.0 "finish"

#Run the simulation
$ns run