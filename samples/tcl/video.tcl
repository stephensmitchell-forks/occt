pload ALL

vclear
vclose all

# Declaration global variables
global startTimeTyre
global currentTimeTyre
global endTimeTyre
global stepTimeTyre
global index

global paveStartKd
global paveStartKt
global paveCurrentKd
global paveCurrentKt
global paveDeltaKd
global paveDeltaKt

global folderTyre
global folderTyre
global folderVideo

global width
global height

set index 0
set startTimeTyre   4805
set currentTimeTyre 4805
set endTimeTyre     5690
set stepTimeTyre    15
set SamplesPerPixel 1

set folderTyre  "D:/TmpFiles/for_video/Tyre"
set folderEnv   "D:/TmpFiles/for_video/Environment"
set folderVideo "D:/TmpFiles/for_video/HighQuality"

#set width 1280
#set height 720
set width 512
set height 512

# Settings
set isEditLight 1
set isGI 1
set isAnim 1

set paveStartKd 0.243137
set paveStartKt 0.0
set paveCurrentKd ${paveStartKd}
set paveCurrentKt ${paveStartKt}
set paveDeltaKd 0.0020448142
set paveDeltaKt 0.0114285714

vinit name=View1 w=${width} h=${height} t=0 l=0
vsetdispmode 1
vcamera -persp -fovy 60

#vzbufftrihedron

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
vsetmaterial  urn aluminium
vbsdf         urn -roughness 128
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
#Tree00
vdisplayobj   tree00 "${folderEnv}/Scene Tree00.obj"
#Tree01
vdisplayobj   tree01 "${folderEnv}/Scene Tree01.obj"

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
vdisplayobj   test "${folderEnv}/Scene Pave10test.obj"
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

vdisplayobj   tyre "${folderTyre}/tyre_3.4805.obj"
vbsdf         tyre -ks 0.3

if { ${isEditLight} == 1 } {
  vlight change 0 head 0
  vlight change 0 sm 0.1
  vlight change 0 int 300
  vlight change 0 direction 1 0.2 -1
}

vtextureenv on "${folderEnv}/sky_midafternoon.jpg"
vrenderparams -env on

if { ${isGI} == 1 } {
  vrenderparams -ray -gi
  vrenderparams -brng
  vrenderparams -rayDepth 5
}

# Animation of movement of the camera.
# Prepearing.
proc StartMovementOfCamera {thePts theLocalPts theName} {
  vviewparams -scale 0.76084571141153157
  vviewparams -proj -0.55490690725196157 0.83079330518538119 0.043137087792445204
  vviewparams -up 0.0038687080384806026 -0.049275302086848008 0.99877774189374235
  vviewparams -at 904 -0.0925 3.5
  vviewparams -eye -1875.1169546272422 4160.7348535943329 219.54166765447638
}
# Main animation.
proc MovementOfCamera {thePts theLocalPts theName} {
  global index
  global folderVideo
  global SamplesPerPixel

  if { ${theLocalPts} < 3.0 } {
    vshiftcam 0.01
  } else {
    vshiftcam 0.005
  }
  
  puts $index
  vrenderparams -spp ${SamplesPerPixel}
  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1

  set index [expr {${index} + 1}]
}

# Animation of rotation of the camera.
# Preparing.
proc StartRotationOfCamera {thePts theLocalPts theName} {
  global index
  set index 91

  vviewparams -scale 0.17291948306641733
  vviewparams -proj -0.5549068942726707 0.8307933138774608 0.043137087351443816
  vviewparams -up 0 0 1
  vviewparams -at 904 -0.0925 3.5
  vviewparams -eye 223.11633760908609 1019.310356524007 56.430205481013957
}
# Main animation.
global z
set z 3.5
proc RotationOfCamera {thePts theLocalPts theName} {
  global z
  global width
  global height
  global index
  global folderVideo
  global SamplesPerPixel

  puts $index

  vmoveto ${width}/2 ${height}/2
  #vrotate -0.0349066 0 0
  vrotate -0.1396264 0 0
  #vrotate 0 -0.004363323 01
  vrotate 0 -0.017453292 01
  vviewparams -at 904 -0.0925 ${z} -up 0 0 1

  vrenderparams -spp ${SamplesPerPixel}
  vfps 1
  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1

  set index [expr {${index} + 1}]
  set z [expr {${z} - 1.5}]
}

# Animation of deformation of tyre.
# Preparing.
proc StartDeformationOfTyre {thePts theLocalPts theName} {
  global index
  set index 111
  
  vviewparams -scale 0.70209376404101775
  vviewparams -proj 0.29616790541429666 -0.88265982860132486 0.36496054413592205
  vviewparams -up -0.11609767165456701 0.34600221479170257 0.9310229846763316
  vviewparams -at 904 -0.092499999999972715 -146.5
  vviewparams -eye 1269.320051268 -1088.8442787782369 425.17506035310009
  
  #vviewparams -scale 0.68818212755896502
  #vviewparams -proj 0.34824854701750618 -0.8177652576045038 0.45823894744470978
  #vviewparams -up 0 0 1
  #vviewparams -at 904 -0.092499999999972715 -146.5
  #vviewparams -eye 1342.2445815876536 -1029.1885184186783 460.15922086382318
}
# Main animation.
proc DeformationOfTyre {thePts theLocalPts theName} {
  global index
  global folderTyre
  global folderVideo
  global SamplesPerPixel
  global currentTimeTyre
  global stepTimeTyre
  global endTimeTyre

  if { ${currentTimeTyre} <= ${endTimeTyre} } {
    #erase previous tyre
    vremove tyre

    #loading current tyre
    puts "loading tyre_3.${currentTimeTyre}..."
    vrenderparams -spp 1
    vdisplayobj tyre "${folderTyre}/tyre_3.${currentTimeTyre}.obj"
    vbsdf       tyre -ks 0.3
    set bbox [vbounding -print tyre]
    regexp {tyre+\n([-0-9.+eE]+) +([-0-9.+eE]+) +([-0-9.+eE]+) +([-0-9.+eE]+) +([-0-9.+eE]+) +([-0-9.+eE]+)} ${bbox} full x1 y1 z1 x2 y2 z2
    vviewparams -at [expr {($x2 + $x1) * 0.5}] [expr {($y2 + $y1) * 0.5}] -131.5 -up 0 0 1
    if { ${currentTimeTyre} <= 4955 } {
      vshiftcam 0.01
    } elseif { ${currentTimeTyre} < 5300 } {
      vshiftcam 0.005
    }
    vrotate -0.003 0 0 

    puts $index
    vrenderparams -spp ${SamplesPerPixel}
    vfps 1
    vdump "${folderVideo}/Res_${index}.png"
    vrenderparams -spp 1
    
    vrenderparams -spp 1
    set index [expr {$index + 1}]

    set currentTimeTyre [expr ${currentTimeTyre} + ${stepTimeTyre}]
  }
}

# Rotation of the camera after deformation.
# Preparing.
proc StartRotationOfCameraAfterDeformation {thePts theLocalPts theName} {
  global folderTyre
  global endTimeTyre
  global paveStartKd
  global paveStartKt
  global index
  set index 171
  
  vrenderparams -gi off
  vremove tyre
  vdisplayobj tyre "${folderTyre}/tyre_3.${endTimeTyre}.obj"
  vbsdf       tyre -ks 0.3
  vrenderparams -gi on
  
  vviewparams -scale 0.89917874503252559
  vviewparams -proj 0.12928743010777263 -0.87670185844940729 0.46333423336451535
  vviewparams -up -0.066045096204520176 0.45860503872268371 0.88618252280526355
  vviewparams -at 1262.0869750976562 14.405502319335938 -131.5
  vviewparams -eye 1386.6074883792353 -829.97175321148939 314.75078022962293
  
  #vviewparams -scale 0.9280456841673036
  #vviewparams -proj 0.18840463572798008 -0.84034129855590489 0.50826193559776089
  #vviewparams -up -0.10946425274888763 0.49633667496588818 0.86120118581777039
  #vviewparams -at 1262.0869750976562 14.405502319335938 -131.5
  #vviewparams -eye 1437.9007346084109 -769.77670424046596 342.79534506079335
  
  vsetmaterial pave10 glass
  vbsdf pave10 -absorpcoeff 1.0 -absorpcolor 1.0 1.0 1.0 -fresnel Constant 0.0 -kt ${paveStartKt} -kd ${paveStartKd}
}
# Main animation.
proc RotationOfCameraAfterDeformation {thePts theLocalPts theName} {
  global index
  global folderVideo
  global SamplesPerPixel

  puts $index

  vrotate 0 0.007853985 0
  
  vrenderparams -spp ${SamplesPerPixel}
  vfps 1
  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1

  set index [expr {$index + 1}]
}

# Idle animation.
# Preparing.
proc StartPaveTransparency {thePts theLocalPts theName} {
  global folderTyre
  global endTimeTyre
  global index
  set index 252

  vrenderparams -gi off
  vremove tyre
  vdisplayobj tyre "${folderTyre}/tyre_3.${endTimeTyre}.obj"
  vbsdf       tyre -ks 0.3
  vrenderparams -gi on

  vviewparams -scale 0.89917874503252926
  vviewparams -proj 0.14323443575502989 -0.97766449080860029 -0.15380520090653158
  vviewparams -up 0.023687355221058894 -0.15197656682365501 0.98810021371272183
  vviewparams -at 1262.0869750976562 14.405502319335938 -131.5
  vviewparams -eye 1400.0402582973147 -927.21182359687623 -279.63429694048989

  #vviewparams -scale 0.92804568416730804
  #vviewparams -proj 0.21658311425190047 -0.97083411217050375 -0.10282451685788761
  #vviewparams -up 0.023884841470820248 -0.10002361570077746 0.99469834153378234
  #vviewparams -at 1262.0869750976562 14.405502319335938 -131.5
  #vviewparams -eye 1464.1960761611324 -891.54882551588094 -227.45286659911696
}
# Main animation.
proc PaveTransparency {thePts theLocalPts theName} {
  global index
  global folderVideo
  global paveStartKd
  global paveStartKt
  global paveCurrentKd
  global paveCurrentKt
  global paveDeltaKd
  global paveDeltaKt
  global SamplesPerPixel

  puts $index

  set paveCurrentKd [expr {$paveCurrentKd - $paveDeltaKd}]
  set paveCurrentKt [expr {$paveCurrentKt + $paveDeltaKt}]
  
  vbsdf pave10 -kd ${paveCurrentKd} -kt ${paveCurrentKt}
  
  vrenderparams -spp ${SamplesPerPixel}
  vfps 1
  vdump "${folderVideo}/Res_${index}.png"
  vrenderparams -spp 1
  set index [expr {$index + 1}]
}

# Idle animation.
# Preparing.
proc StartIdleAnim {thePts theLocalPts theName} {
  global index
  global endTimeTyre
  global folderTyre
  set index 313
  
  vbsdf pave10 -absorpcoeff 1.0 -absorpcolor 1.0 1.0 1.0 -fresnel Constant 0.0 -kt 0.8 -kd 0.1

  vrenderparams -gi off
  vremove tyre
  vdisplayobj tyre "${folderTyre}/tyre_3.${endTimeTyre}.obj"
  vbsdf       tyre -ks 0.3
  vrenderparams -gi on

  vviewparams -scale 0.89917874503252926
  vviewparams -proj 0.14323443575502989 -0.97766449080860029 -0.15380520090653158
  vviewparams -up 0.023687358041704527 -0.15197658016980603 0.98810021159237427
  vviewparams -at 1262.0869750976562 14.405502319335938 -131.5
  vviewparams -eye 1400.0402582973147 -927.21182359687623 -279.63429694048989

  #vviewparams -scale 0.92804568416730826
  #vviewparams -proj 0.21658311425190027 -0.97083411217050386 -0.10282451685788763
  #vviewparams -up 0.023884838973521033 -0.10002359984931483 0.99469834318771899
  #vviewparams -at 1262.0869750976562 14.405502319335938 -131.5
  #vviewparams -eye 1464.1960761611322 -891.54882551588094 -227.45286659911696
}
# Main idle animation.
proc IdleAnim {thePts theLocalPts theName} {
  global index
  global folderVideo
  global SamplesPerPixel

  puts $index
  #vrenderparams -spp ${SamplesPerPixel}
  #vfps 1
  #vdump "${folderVideo}/Res_${index}.png"
  #vrenderparams -spp 1
  set index [expr {$index + 1}]
}
  
if { ${isAnim} == 1 } {
  set startTimeMovCam 0.0
  set endTimeMovCam 4.5
  set endTimeRotation 4.8
  set endTimeDeformations 8.0

  vlight change 0 head 0
  vrenderparams -spp 1

  vanim movcam -reset -onRedraw MovementOfCamera -onStart StartMovementOfCamera
  vanim animcam0 -reset -addSlice 0.0 4.5 movcam
  #vanim -play animcam0 -playFps 20
  
  vanim rotcam -reset -onRedraw RotationOfCamera -onStart StartRotationOfCamera
  vanim animcam1 -reset -addSlice 0.0 1.0 rotcam
  #vanim -play animcam1 -playFps 20
  
  vanim deftyre -reset -onRedraw DeformationOfTyre -onStart StartDeformationOfTyre
  vanim animdef -reset -addSlice 0.0 3.0 deftyre
  #vanim -play animdef -playFps 20
  
  vanim rotCamAfterDef -reset -onRedraw RotationOfCameraAfterDeformation -onStart StartRotationOfCameraAfterDeformation
  vanim animRotCamAfterDef -reset -addSlice 0.0 4.0 rotCamAfterDef
  #vanim -play animRotCamAfterDef -playFps 20
  
  vanim pavetrans -reset -onRedraw PaveTransparency -onStart StartPaveTransparency 
  vanim animpavetrans -reset -addSlice 0.0 3.0 pavetrans
  #vanim -play animpavetrans -playFps 20
  
  vanim idle -reset -onRedraw IdleAnim -onStart StartIdleAnim 
  vanim animidle -reset -addSlice 0.0 2.0 idle
  vanim -play animidle -playFps 20
  
  vrenderparams -spp 1
}