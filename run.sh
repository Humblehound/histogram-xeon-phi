#!/bin/sh
set -e
echo "Copying files..."
sshpass -p student scp main.cpp student12@apl12.eti.pg.gda.pl:~/143297/
echo "Compiling..."
sshpass -p 'student' ssh student12@apl12.eti.pg.gda.pl 'cd 143297 && source /opt/intel/composer_xe_2013_sp1.3.174/bin/compilervars.sh intel64 && icpc -g -openmp BmpImage.cpp main.cpp'
echo "Running..."
sshpass -p 'student' ssh student12@apl12.eti.pg.gda.pl 'cd 143297 && source /opt/intel/composer_xe_2013_sp1.3.174/bin/compilervars.sh intel64 && ./a.out'
echo "Done..."
