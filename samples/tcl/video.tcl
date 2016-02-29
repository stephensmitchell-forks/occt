pload ALL

vclear
vclose all

# Declaration global variables
global startTimeTyre
global currentTimeTyre
global endTimeTyre
global stepTimeTyre

set startTimeTyre   4805
set currentTimeTyre 4805
set endTimeTyre     5690
set stepTimeTyre    15
set SamplesPerPixel 30

global folderTyre
global folderTyre
global folderVideo

set folderTyre  "D:/TmpFiles/for_video/Tyre"
set folderEnv   "D:/TmpFiles/for_video/Environment"
set folderVideo "D:/TmpFiles/for_video/Result"

global width
global height
set width 512
set height 512

# Settings
set isEditLight 1
set isGI 1
set isAnim 1

vinit name=View1 w=${width} h=${height} t=256 l=1024
vsetdispmode 1
vcamera -persp -fovy 60

vzbufftrihedron

#building0
puts "Loading the first building..."
vdisplayobj   building0 "${folderEnv}/Scene Building0.obj"
#grass0
puts "Loading the first grass..."
vdisplayobj   grass0 "${folderEnv}/Scene Grass0.obj"
vsetmaterial  grass0 plastic
vbsdf grass0 -kd 0.113 0.306 0.008
#bench0
puts "Loading the first bench..."
vdisplayobj   bench0 "${folderEnv}/Scene Bench0.obj"
#bench1
puts "Loading the second bench..."
vdisplayobj   bench1 "${folderEnv}/Scene Bench1.obj"
#urn
puts "Loading the urn..."
vdisplayobj   urn "${folderEnv}/Scene Urn.obj"
vsetmaterial  urn steel
#pave00
puts "Loading the first pave..."
vdisplayobj   pave00 "${folderEnv}/Scene Pave00.obj"
vsetmaterial  pave00 stone
#pavement0
puts "Loading the first pavement..."
vdisplayobj   pavement0 "${folderEnv}/Scene Pavement0.obj"
#pave01
puts "Loading the second pave..."
vdisplayobj   pave01 "${folderEnv}/Scene Pave01.obj"
vsetmaterial  pave01 stone
#Tree0
vdisplayobj   tree0 "${folderEnv}/Scene Tree00.obj"

#building1
puts "Loading the second building..."
vdisplayobj   building1 "${folderEnv}/Scene Building1.obj"
#grass1
puts "Loading the second grass..."
vdisplayobj   grass1 "${folderEnv}/Scene Grass1.obj"
vsetmaterial  grass1 plastic
vbsdf grass1 -kd 0.113 0.306 0.008
#pave10
puts "Loading the second pave..."
vdisplayobj   pave10 "${folderEnv}/Scene Pave10.obj"
vsetmaterial  pave10 stone
#pavement
vdisplayobj   pavement1 "${folderEnv}/Scene Pavement1.obj"
#pave11
vdisplayobj   pave11 "${folderEnv}/Scene Pave11.obj"
vsetmaterial  pave11 stone
#Tree10
vdisplayobj   tree1 "${folderEnv}/Scene Tree10.obj"

#building2
puts "Loading the third building..."
vdisplayobj   building2 "${folderEnv}/Scene Building2.obj"
#grass2
puts "Loading the third grass..."
vdisplayobj   grass2 "${folderEnv}/Scene Grass2.obj"
vsetmaterial  grass2 plastic
vbsdf grass2 -kd 0.113 0.306 0.008

#The ground
puts "Loading the ground..."
vdisplayobj   ground "${folderEnv}/Scene Ground 1.obj"

if { ${isEditLight} == 1 } {
  vlight change 0 head 0
  vlight change 0 sm 0.1
  vlight change 0 int 300
  vlight change 0 direction 1 0.2 -1
}

vtextureenv on "${folderEnv}/sky_midafternoon.jpg"
vrenderparams -ray -env on

if { ${isGI} == 1 } {
  vrenderparams -gi
  vrenderparams -brng
  #vrenderparams -filter on
}

vdisplayobj   tyre "${folderTyre}/tyre_3.5690.obj"

global index
set index 0
if { 0 == 1 } {
  #start position
  puts "begin"
  vviewparams -proj -0.20622021944287358 0.47082896669284285 0.85778395019718279
  vviewparams -up 0.34414165473972619 -0.78572247727332134 0.51401041835791439
  vviewparams -at -13354.357466784533 27218.390422097546 26205.999398111326
  vviewparams -eye -15033.237088976141 31051.502227786012 33189.388311767325
  vrenderparams -spp $SamplesPerPixel
  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1
  set index [expr {$index + 1}]

  #knot0
  puts "knot 0"
  vviewparams -proj -0.20622021944287344 0.47082896669284346 0.85778395019718257
  vviewparams -up 0.34414165473972619 -0.78572247727332134 0.51401041835791439
  vviewparams -at -13354.357466784533 27218.390422097546 26205.999398111326
  vviewparams -eye -14254.556394169076 29273.667697737732 29950.424624606083
  vrenderparams -spp $SamplesPerPixel
  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1
  set index [expr {$index + 1}]

  #knot1
  puts "knot 1"
  vviewparams -proj -0.41571695570910722 0.43430578177976742 0.79909817960537066
  vviewparams -up 0.55255880993299089 -0.57726654320370485 0.60120054862174055
  vviewparams -at -21230.015657075473 24204.803919974482 23556.870926717536
  vviewparams -eye -23044.716220039514 26100.649003680352 27045.119112120185
  vrenderparams -spp $SamplesPerPixel
  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1
  set index [expr {$index + 1}]

  #knot2
  puts "knot 2"
  vviewparams -proj -0.44762421794647089 0.53801207252628835 0.71426575539065151
  vviewparams -up 0.45682847922240011 -0.54907495291086994 0.69987458637764166
  vviewparams -at -22333.259756577103 25954.230943443392 21873.264333951254
  vviewparams -eye -22341.274931712684 25963.864606711519 21886.054001921115
  vrenderparams -spp $SamplesPerPixel
  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1
  set index [expr {$index + 1}]

  #knot3
  puts "knot 3"
  vviewparams -proj -0.44788044499548108 0.53779655646484081 0.71426743650064939
  vviewparams -up 0.4570921661291803 -0.54885765333719594 0.69987286562383633
  vviewparams -at -13707.386192562162 14482.648377528752 10849.765096470677
  vviewparams -eye -15495.281424403387 16629.479992411649 13701.051323070391
  vrenderparams -spp $SamplesPerPixel
  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1
  set index [expr {$index + 1}]

  #knot4
  puts "knot 4"
  vviewparams -proj -0.44718015336668421 0.53800674424855244 0.71454786654080893
  vviewparams -up 0.45727400220364306 -0.54907136249035604 0.69958639623826779
  vviewparams -at 11435.8 -15967.5 -28391.8
  vviewparams -eye -13484.174719894208 14002.468293369575 11428.231554346788
  vrenderparams -spp $SamplesPerPixel
  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1
  set index [expr {$index + 1}]

  #knot5
  puts "knot 5"
  vviewparams -proj -0.54095886294668194 0.72231331411763966 0.43084450193523982
  vviewparams -up 0.28959797496449069 -0.32097294312176605 0.90172578020161875
  vviewparams -at 17028.225461231897 -23490.280235621023 -17394.904746724318
  vviewparams -eye -6852.4221626059334 8396.2658771283459 1624.7406202792517
  vrenderparams -spp $SamplesPerPixel
  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1
  set index [expr {$index + 1}]

  #knot6
  puts "knot 6"
  vviewparams -proj -0.54095886294668194 0.72231331411763966 0.43084450193523988
  vviewparams -up 0.28959797496449069 -0.32097294312176605 0.90172578020161875
  vviewparams -at 18537.604312086372 -23905.520476230522 -14803.609241620072
  vviewparams -eye -1565.2364759343 2936.7230793679955 1207.2171262471238
  vrenderparams -spp $SamplesPerPixel
  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1
  set index [expr {$index + 1}]

  #knot7
  puts "knot 7"
  vviewparams -proj -0.62110268896811893 0.64641254663814718 0.44315039129550315
  vviewparams -up 0.29662780874258493 -0.32948539230542279 0.89635446076756597
  vviewparams -at 20691.468156274099 -22215.566058836219 -14958.801037599358
  vviewparams -eye -2389.6365205911934 1806.0907756908309 1509.3301599623119
  vrenderparams -spp $SamplesPerPixel
  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1
  set index [expr {$index + 1}]

  #knot8
  puts "knot 8"
  vviewparams -proj -0.87404055231429312 0.20603968241462572 0.44000086611347194
  vviewparams -up 0.35531317939173412 -0.34659785485005534 0.86811431940953332
  vviewparams -at 30853.661242490405 -7437.5407276112601 -15288.204166516902
  vviewparams -eye -1626.9911814297993 219.20161955377262 1062.8852201417812
  vrenderparams -spp $SamplesPerPixel
  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1
  set index [expr {$index + 1}]

  #knot9
  puts "knot 9"
  vviewparams -proj -0.75181851825962331 -0.31237650141434464 0.58068049473530503
  vviewparams -up 0.53944967908272923 0.21501377057130752 0.81410264844321056
  vviewparams -at 26512.037657317873 10567.979608932925 -19976.566463759911
  vviewparams -eye -289.56414163475711 -567.94002980904588 724.13059853308005
  vrenderparams -spp $SamplesPerPixel
  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1
  set index [expr {$index + 1}]

  #knot10
  puts "knot 10"
  vviewparams -proj -0.31369817861305538 -0.76634695237879957 0.5606298255663692
  vviewparams -up 0.21238458206350391 0.51884353282430495 0.82806652978375184
  vviewparams -at 13086.171886955321 29646.307936948524 -21897.75321880031
  vviewparams -eye 317.39012410108262 -1547.1055307894094 922.14303961578844
  vrenderparams -spp $SamplesPerPixel
  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1
  set index [expr {$index + 1}]
}
#end position
puts "end position"
vviewparams -proj 0.54593763262437844 -0.71028743797754057 0.44434655026880843
vviewparams -up 0 0 0.1
vviewparams -at 1261.5 29 1.5
vviewparams -eye 1656.5277404797348 -782.74632298760184 468.51284756718968

vrenderparams -spp $SamplesPerPixel
vdump "${folderVideo}/Res_${index}.png"
vrenderparams -spp 1
set index [expr {$index + 1}]

# deformations
set test 0
if { ${test} == 1 } {
  set tyres [glob -type f "${folderTyre}/tyre_*.obj"]
  foreach mesh $tyres {
    #erase previous tyre
    vremove tyre

    #loading current tyre
    puts "loading tyre_3.${currentTimeTyre}..."
    vrenderparams -spp 1
    vdisplayobj tyre ${mesh}
    vrenderparams -spp $SamplesPerPixel
    vfps 1
    #vrenderparams -spp $SamplesPerPixel
    vdump "${folderVideo}/Res_${index}.png"
    vrenderparams -spp 1
    set index [expr {$index + 1}]

    set currentTimeTyre [expr ${currentTimeTyre} + ${stepTimeTyre}]
    set prevTimeTyre [expr ${currentTimeTyre} - ${stepTimeTyre}]
  }
}

proc Anim0 {thePts theLocalPts theName} {
  global width
  global height
  global index
  global folderVideo
  global SamplesPerPixel

  vmoveto ${width}/2 ${height}/20
  vrenderparams -spp ${SamplesPerPixel}
  vrotate 0.0872665 0 0

  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1
  set index [expr {$index + 1}]
}

if { ${isAnim} == 1 } {
  vrenderparams -spp 1

  vanim anim0 -reset -onRedraw Anim0
  vanim anim -reset -addSlice 0.0 3.5 anim0
  #vanim -play anim -playFps 20 -record "${folderVideo}/video.mkv" -recWidth 1920 -recHeight 1080
  vanim -play anim -playFps 20

  vrenderparams -spp 1
}