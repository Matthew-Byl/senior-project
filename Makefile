all:
	$(MAKE) -C cl_test
	$(MAKE) -C raytracer
	$(MAKE) -C kalah

clean:
	$(MAKE) -C cl_test clean
	$(MAKE) -C raytracer clean
	$(MAKE) -C kalah clean
