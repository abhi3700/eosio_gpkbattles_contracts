# FAQs
1. Why `transferbypl` & `sel3card` ACTION are splitted?
	- As the `sel3card` ACTION can't be removed during the draw case in result. There, the cards remain in escrow contract as "available" status.
	- So, the procedure is `transferbypl` >> `sel3card`.

1. Can cards transferred using `transferbypl` be of any no.?
	- Yes, it can be of 1 to any no.
	- It has been kept like this in order to transfer different type `a`/`b` cards after the result is draw.