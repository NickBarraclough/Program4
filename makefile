key: keygen.c
	gcc -std=gnu11 -o keygen keygen.c
otp: otp.c
	gcc -std=gnu11 -o otp otp.c
otp_d: otp_d.c
	gcc -std=gnu11 -o otp_d otp_d.c
all: keygen.c otp.c otp_d.c
	gcc -std=gnu11 -o keygen keygen.c
	gcc -std=gnu11 -o otp otp.c
	gcc -std=gnu11 -o otp_d otp_d.c
clean:
	rm -rf *.o keygen client server otp otp_d 