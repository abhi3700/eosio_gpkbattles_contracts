# FAQs
1. Why `transfer` & `select` ACTION are splitted?
	- As the `select` ACTION can't be removed during the draw case in result. There, the cards remain in escrow contract as "available" status.
	- So, the procedure is `transfer` >> `select`.