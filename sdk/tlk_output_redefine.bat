echo "*************** start of post build *****************"


cd ../
if exist output (rm -rf output)
mkdir output
cd output
mkdir objlist
mkdir objdump


cp -r  ../tlkapp_general/app     ./objlist/app
cp -r  ../tlkapp_general/core    ./objlist/core
cp -r  ../tlkapp_general/tlklib  ./objlist/tlklib
cp -r  ../tlkapp_general/tlkapi  ./objlist/tlkapi
cp -r  ../tlkapp_general/tlkalg  ./objlist/tlkalg
cp -r  ../tlkapp_general/tlkdrv  ./objlist/tlkdrv
cp -r  ../tlkapp_general/tlkdev  ./objlist/tlkdev
cp -r  ../tlkapp_general/tlkstk  ./objlist/tlkstk
cp -r  ../tlkapp_general/tlkprt  ./objlist/tlkprt
cp -r  ../tlkapp_general/tlkmdi  ./objlist/tlkmdi
cp -r  ../tlkapp_general/tlkmmi  ./objlist/tlkmmi

cp -r  ../tlkapp_general/output/*.*  ./objdump
cp -r  ../tlkapp_general/makefile    ./
cp -r  ../tlkapp_general/*.*         ./


echo "rm -rf ../tlkapp_general"

echo "**************** end of post build ******************"



