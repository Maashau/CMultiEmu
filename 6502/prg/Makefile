ASMDIR=./asm
OBJDIR=./obj
BINDIR=./bin
LSTDIR=./lst
MAPDIR=./map
CC65DIR=

PRGLIST=\
blink\
weekday\
kbTest\
memClear\
memWrite\
numQuery\
print

all : testPrgs

testPrgs : $(OBJDIR) $(LSTDIR) $(BINDIR) $(MAPDIR) $(PRGLIST)

blink : $(BINDIR)/blink.rom
weekday : $(BINDIR)/weekday.rom
kbTest : $(BINDIR)/kbTest.rom
memClear : $(BINDIR)/memClear.rom
memWrite : $(BINDIR)/memWrite.rom
numQuery : $(BINDIR)/numQuery.rom
print : $(BINDIR)/print.rom

$(BINDIR)/%.rom : $(ASMDIR)/%.asm
	$(info )
	$(info Assembling "$<")
	@$(CC65DIR)ca65 -t none $< -l $(LSTDIR)/$(basename $(notdir $<)).lst -o $(OBJDIR)/$(basename $(notdir $<)).o
	@rm -rf $@
	@$(CC65DIR)ld65 -C $(ASMDIR)/custom.cfg $(OBJDIR)/$(basename $(notdir $<)).o -o $@ -m $(MAPDIR)/$(basename $(notdir $<)).map

$(LSTDIR) :
	@mkdir -p $@

$(OBJDIR) :
	@mkdir -p $@

$(BINDIR) :
	@mkdir -p $@
	
$(MAPDIR) :
	@mkdir -p $@
