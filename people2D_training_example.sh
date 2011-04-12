MACHINE_TYPE=`uname -m`
if [ ${MACHINE_TYPE} == 'x86_64' ]; then
	echo '64Bit machine detected'
	TRAINING_BIN=./external_bin/adaboost_train64
else
	echo '32Bit machine detected'
	TRAINING_BIN=./external_bin/adaboost_train32
fi

DUMPFEAT_BIN=./people2D_dumpfeats
DATA_TRAINING=data/training.dat
OUTDIR=model/
#~ segmentation distance
THRESHOLD=0.2
#~ num of weak classifiers
NWEAK=50
#~ 
mkdir $OUTDIR
$DUMPFEAT_BIN -i $DATA_TRAINING -d $THRESHOLD -o $OUTDIR/ppl2Dindoor.abt
$TRAINING_BIN -i $OUTDIR/ppl2Dindoor.abt -o $OUTDIR/ppl2Dindoor -n $NWEAK
