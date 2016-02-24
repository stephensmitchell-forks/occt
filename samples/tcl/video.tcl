pload ALL

vclear
vclose all

vinit name=View1 w=512 h=512 t=256 l=1024
vsetdispmode 1
vcamera -persp -fovy 60

vtrihedron tr0
vzbufftrihedron

# Declaration global variables
global startTimeTyre
global currentTimeTyre
global endTimeTyre
global stepTimeTyre

set startTimeTyre   4805
set currentTimeTyre 4805
set endTimeTyre     5690
set stepTimeTyre    15

global folderTyre
global folderTyre
global folderVideo

set folderTyre  "D:/TmpFiles/for_video/Tyre"
set folderEnv   "D:/TmpFiles/for_video/Environment"
set folderVideo "D:/TmpFiles/for_video/Result"

# Settings
set isEditLight 1
set isGI 1
set isAnim 1

#road
puts "Loading road..."
#vdisplayobj   road "${folderEnv}/roadV2.obj"
#vlocrotate    road 0 0 0 1 0 0 90
#vlocscale     road 1 1 0 102400
#vloctranslate road 0 1.0008 3.5

#building0
puts "Loading the first building..."
#vdisplayobj   building0 "${folderEnv}/Building N100715.obj"
#vlocscale     building0 1 1 1 10922
#vlocrotate    building0 0 0 0 1 0 0 90
#vlocrotate    building0 0 0 0 0 1 0 45
#vloctranslate building0 -6 3 3


#House N090614
puts "Loading the second building..."
#vdisplayobj   building1 "${folderEnv}/Scene BuildingN090614.obj"
#vlocscale     building1 1 1 1 21845
#vloctranslate building1 -1 1 1
#vlocrotate    building1 0 0 0 0 0 1 180

#Building1
puts "Loading the third building..."
#vdisplayobj   building2 "${folderEnv}/Scene BuildingN101213.obj"
#vlocscale     building2 1 1 1 21845
#vloctranslate building2 -1 1 1
#vlocrotate    building2 0 0 0 0 0 1 180

#lawn
puts "Loading lawn..."
#vdisplayobj   lawn "${folderEnv}/Scene Lawn.obj"
#vlocscale     lawn 1 1 1 21485
#vloctranslate lawn -1 1 1
#vlocrotate    lawn 0 0 0 0 0 1 180
#vsetmaterial  lawn plastic
#vbsdf         lawn -kd 0.114 0.306 0.008

#bench0
puts "Loading the first bench..."
#vdisplayobj   bench0 "${folderEnv}/Scene BenchN210815.obj"
#vlocscale     bench0 1 1 1 21485
#vloctranslate bench0 -1 1 1
#vlocrotate    bench0 0 0 0 0 0 1 180

#urn0
puts "Loading the first urn..."
vdisplayobj   urn0 "${folderEnv}/Scene Urn.obj"
vlocscale     urn0 1 1 1 21485
vloctranslate urn0 -1 1 1
vlocrotate    urn0 0 0 0 0 0 1 180

vviewparams -eye 24631.990743707178 -5440.5284854311521 10802.013424258917
vviewparams -at 5913.1650096036392 5334.3047672616967 5730.8013468954196
vviewparams -proj 0.84373094111182723 -0.48566402239936912 0.22857943117746968
vviewparams -up -0.41631981347694685 -0.32330155298659913 0.84979639840550847

if { ${isEditLight} == 1 } {
  vlight change 0 head 0
  vlight change 0 sm 0.1
  vlight change 0 int 300
  vlight change 0 direction 1 1 -1
}

vtextureenv on 4
vrenderparams -ray -env on

if { ${isGI} == 1 } {
  vrenderparams -gi
  vrenderparams -brng
}

proc Anim0 {thePts theLocalPts theName} {
  global startTimeTyre
  global currentTimeTyre
  global endTimeTyre
  global stepTimeTyre
  global folderTyre

  if { ${currentTimeTyre} > ${startTimeTyre} } {
    set prevTimeTyre [expr ${currentTimeTyre} - ${stepTimeTyre}]
    vremove tyre_${prevTimeTyre}
  }

  if { ${currentTimeTyre} <= ${endTimeTyre} } {
    puts "Loading tyre_${currentTimeTyre}..."
    vdisplayobj tyre_${currentTimeTyre} "${folderTyre}/tyre_3.${currentTimeTyre}.obj"
  }

  set currentTimeTyre [expr ${currentTimeTyre} + ${stepTimeTyre}]
}

if { ${isAnim} == 1 } {
  vanim anim0 -reset -onRedraw Anim0
  vanim anim -reset -addSlice 0.0 0.5 anim0
  vanim -play anim -playFps 60 -record "${folderVideo}/video.mkv" -recWidth 1920 -recHeight 1080
}
