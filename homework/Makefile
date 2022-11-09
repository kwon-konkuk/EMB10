obj-m += seg_driver.o button_driver.o
RESULT = homework
SRC = homework.c

all:
	make -C /home/kwon/working/kernel M=$(PWD) modules
	aarch64-linux-gnu-gcc -o $(RESULT) $(SRC)

clean:
	make -C $(HOME)/working/kernel M=$(PWD) clean
	rm -f $(RESULT)
