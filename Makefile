obj-m += seg_driver.o 
RESULT1 = seg_example 
RESULT2 = seg_example2
SRC1 = seg_example.c 
SRC2 = seg_example2.c

all:
	make -C /home/kwon/working/kernel M=$(PWD) modules
	aarch64-linux-gnu-gcc -o $(RESULT1) $(SRC1)
	make -C /home/kwon/working/kernel M=$(PWD) modules
	aarch64-linux-gnu-gcc -o $(RESULT2) $(SRC2)

clean:
	make -C $(HOME)/working/kernel M=$(PWD) clean
	rm -f $(RESULT1)
	make -C $(HOME)/working/kernel M=$(PWD) clean
	rm -f $(RESULT2)
