make
rm boot.dol
rm boot.elf
mv $1.dol boot.dol
mv $1.elf boot.elf
mkdir -p f:/apps/$1/
cp *.* F:/apps/$1/