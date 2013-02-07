resource-extraction.cl: resource-extraction.cl.in effort.cl utility.cl ../util/choose_thread.cl ../util/max_index.cl
	cpp -I ../util -I ../mwc64x/cl -I ../config resource-extraction.cl.in > resource-extraction.cl

clean:
	rm -f resource-extraction.cl