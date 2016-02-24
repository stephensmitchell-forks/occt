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

global folderTyre
global folderTyre
global folderVideo

set folderTyre  "D:/TmpFiles/for_video/Tyre"
set folderEnv   "D:/TmpFiles/for_video/Environment"
set folderVideo "D:/TmpFiles/for_video/Result"

set width 512
set height 512

# Settings
set isEditLight 1
set isGI 1
set isAnim 0

vinit name=View1 w=${width} h=${height} t=256 l=1024
vsetdispmode 1
vcamera -persp -fovy 60

vzbufftrihedron

#building0
puts "Loading the first building..."
#vdisplayobj   building0 "${folderEnv}/Building N100715.obj"
#vlocscale     building0 1 1 1 10922
#vlocrotate    building0 0 0 0 1 0 0 90
#vlocrotate    building0 0 0 0 0 1 0 45
#vloctranslate building0 -6 3 3

#House N090614
puts "Loading the second building..."
#height 7000
#vdisplayobj   building1 "${folderEnv}/Scene BuildingN090614.obj"
#vlocscale     building1 1 1 1 15555
#vloctranslate building1 -1 1 1
#vlocrotate    building1 0 0 0 0 0 1 180

#Building1
puts "Loading the third building..."
#vdisplayobj   building2 "${folderEnv}/Scene BuildingN101213.obj"
#vlocscale     building2 1 1 1 15555
#vloctranslate building2 -1 1 1
#vlocrotate    building2 0 0 0 0 0 1 180

#lawn
puts "Loading lawn..."
#vdisplayobj   lawn "${folderEnv}/Scene Lawn.obj"
#vlocscale     lawn 1 1 1 15555
#vloctranslate lawn -1 1 1
#vlocrotate    lawn 0 0 0 0 0 1 180
#vsetmaterial  lawn plastic
#vbsdf         lawn -kd 0.114 0.306 0.008

#bench0
puts "Loading the first bench..."
#vdisplayobj   bench0 "${folderEnv}/Scene BenchN210815.obj"
#vlocscale     bench0 1 1 1 15555
#vloctranslate bench0 -1 1 1
#vlocrotate    bench0 0 0 0 0 0 1 180

#urn0
puts "Loading the first urn..."
#vdisplayobj   urn0 "${folderEnv}/Scene Urn.obj"
#vlocscale     urn0 1 1 1 15555
#vloctranslate urn0 -1 1 1
#vlocrotate    urn0 0 0 0 0 0 1 180

#The ground
puts "Loading the ground..."
#vdisplayobj   ground "${folderEnv}/Scene Ground.obj"
#vlocscale     ground 1 1 1 25000
#vloctranslate ground -1 1 1
#vlocrotate    ground 0 0 0 0 0 1 180

vdisplayobj   building0 "${folderEnv}/Scene Building0.obj"
#vloctranslate building0 21494 886 0

vdisplayobj   building1 "${folderEnv}/Scene Building1.obj"
#vloctranslate building1 21494 886 0

vdisplayobj   pave0 "${folderEnv}/Scene Pave0.obj"
#vloctranslate pave0 21494 886 0

vdisplayobj   pave1 "${folderEnv}/Scene Pave1.obj"
#vloctranslate pave1 21494 886 0

vdisplayobj   ground "${folderEnv}/Scene Ground 1.obj"
#vloctranslate ground 21494 886 0
vviewparams -eye 2791.5312733012979 -725.23102548487986 116.8841126550864
vviewparams -at 1725.2371931049909 335.97379619693942 -283.97309576949539
vviewparams -proj 0.68489870691961574 -0.68162979017282999 0.257477359021686
vviewparams -up -0.077576623573434436 0.2831440641922589 0.95593478145092925

if { ${isEditLight} == 1 } {
  vlight change 0 head 0
  vlight change 0 sm 0.1
  vlight change 0 int 300
  vlight change 0 direction -1 0.2 -1
}

vtextureenv on 4
vrenderparams -ray -env on

if { ${isGI} == 1 } {
  vrenderparams -gi
  vrenderparams -brng
  #vrenderparams -filter on
}

vdisplayobj   tyre_3 "${folderTyre}/tyre_3.5600.obj"
#vloctranslate tyre_3 0 0 500

#vdisplayobj   pave "${folderEnv}/pave.obj"
#vloctranslate pave 0 0 500

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
    #vrender -spp 10
    vloctranslate tyre_${currentTimeTyre} 0 0 500
    #vrender -spp 1
  }

  set currentTimeTyre [expr ${currentTimeTyre} + ${stepTimeTyre}]
}

if { ${isAnim} == 1 } {
  vanim anim0 -reset -onRedraw Anim0
  vanim anim -reset -addSlice 0.0 3.0 anim0
  vanim -play anim -playFps 20 -record "${folderVideo}/video.mkv" -recWidth 1920 -recHeight 1080
}
