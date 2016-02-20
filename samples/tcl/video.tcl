pload ALL

vclear
vclose all

vinit name=View1 w=512 h=512
vsetdispmode 1
vcamera -persp -fovy 90

vzbufftrihedron

set folderTyre "D:/TmpFiles/for_video/Tyre"
set folderEnv  "D:/TmpFiles/for_video/Environment"

#road
puts "Loading road..."
vdisplayobj   road "${folderEnv}/roadV2.obj"
vlocrotate    road 0 0 0 1 0 0 90
vlocscale     road 1 1 0 102400
vloctranslate road 0 1 3.5
#vlocscale     road

#building0
puts "Loading the first building..."
vdisplayobj   building0 "${folderEnv}/Building N100715.obj"
vlocscale     building0 1 1 1 10922
vlocrotate    building0 0 0 0 1 0 0 90
vlocrotate    building0 0 0 0 0 1 0 45
vloctranslate building0 -6 3 3

#House N090614
puts "Loading the second building..."
vdisplayobj   HouseN090614 "${folderEnv}/House N090614.obj"
vloctranslate HouseN090614 0 0 1000
vlocrotate    HouseN090614 0 0 0 1 0 0 90
vlocscale     HouseN090614 1 1 1 1024

#Building1
puts "Loading the third building..."
vdisplayobj   building1 "${folderEnv}/Building N101213.obj"
vlocrotate    building1 0 0 0 1 0 0 90
vlocscale     building1 1 1 1 21845
vloctranslate building1 -1 1 0
vlocrotate    building1 0 0 0 0 1 0 45

#Tyre
puts "Loading tyre..."
vdisplayobj   tyre "${folderTyre}/tyre.obj"
vloctranslate tyre 0 -10000 400

#vlight del 0
#vlight del 1
#vlight add positional head 0 pos 0.5 0.5 50.85
#vlight change 0 sm 0.06
#vlight change 0 int 160.0


#vrenderparams -ray -gi -rayDepth 8
vtextureenv on 4
vrenderparams -ray -env on
vfit
