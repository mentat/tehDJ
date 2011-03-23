# Makefile for resample test - assumes resample is installed on the system

# The 'play' utility below is part of the SoX (Sound eXchange) distribution.
PLAY = play

# Generally set to previous stable release:
TRUTH = resample-1.6

tests: sinetest1p1 sinetestp9 imptest1p1 imptestp9

sinetest1p1:
	-/bin/rm -f siner1p1.snd
	resample -by 1.111111 sine.snd siner1p1.snd | tee >> tests.log
	@(if (! cmp siner1p1.$(TRUTH).snd siner1p1.snd); then echo 'TEST FAILED'; exit -1; \
		else echo 'TEST SUCCEEDED'; fi)
	($(PLAY) sine.snd; $(PLAY) siner1p1.snd)

sinetestp9:
	-/bin/rm -f sinerp9.snd
	resample -by 1.111111 sine.snd sinerp9.snd | tee >> tests.log
	@(if (! cmp sinerp9.$(TRUTH).snd sinerp9.snd); then echo 'TEST FAILED'; exit -1; \
		else echo 'TEST SUCCEEDED'; fi)
	($(PLAY) sine.snd; $(PLAY) sinerp9.snd)

imptest1p1:
	-/bin/rm -f irp1p1.snd
	resample -by 1.111111 i.snd ir1p1.snd | tee >> tests.log
	@(if (! cmp ir1p1.$(TRUTH).snd ir1p1.snd); then echo 'TEST FAILED'; exit -1; \
		else echo 'TEST SUCCEEDED'; fi)
	($(PLAY) i.snd; $(PLAY) ir1p1.snd)

imptestp9:
	-/bin/rm -f irp9.snd
	resample -by 1.111111 i.snd irp9.snd | tee >> tests.log
	@(if (! cmp irp9.$(TRUTH).snd irp9.snd); then echo 'TEST FAILED'; exit -1; \
		else echo 'TEST SUCCEEDED'; fi)
	($(PLAY) i.snd; $(PLAY) irp9.snd)

clean:
	-/bin/rm -f siner1p1.snd sinerp9.snd ir1p1.snd irp9.snd tests.log *~
