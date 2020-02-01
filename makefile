all :
	gcc main.c -o main
	cat inputfile | ./main | diff outputfile -
	cat inputLRU | ./main | diff outputLRU -
	cat inputLRU2 | ./main | diff outputLRU2 -
	cat inputFIFO | ./main | diff outputFIFO -
	cat inputFIFO2 | ./main | diff outputFIFO2 -
	cat inputCLOCK | ./main | diff outputCLOCK -
	
