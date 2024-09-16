ASMDIR=./asm
OBJDIR=./obj
BINDIR=./bin
LSTDIR=./lst
MAPDIR=./map
APPLEDIR=./apple_i
CC65DIR=

PRGLIST=\
blink\
weekday\
kbTest\
memClear\
memWrite\
numQuery\
print

all : testPrgs appleEmu

testPrgs : $(OBJDIR) $(LSTDIR) $(BINDIR) $(MAPDIR) $(PRGLIST)

blink : $(BINDIR)/blink.6502
weekday : $(BINDIR)/weekday.6502
kbTest : $(BINDIR)/kbTest.6502
memClear : $(BINDIR)/memClear.6502
memWrite : $(BINDIR)/memWrite.6502
numQuery : $(BINDIR)/numQuery.6502
print : $(BINDIR)/print.6502

appleEmu : $(APPLEDIR) intBasic wozmon
intBasic : $(APPLEDIR)/$(BINDIR)/intBasic.6502
wozmon : $(APPLEDIR)/$(BINDIR)/wozmon.6502

$(APPLEDIR)/$(BINDIR)/%.6502 : $(APPLEDIR)/$(ASMDIR)/%.asm
	$(info )
	$(info Assembling "$<")
	@$(CC65DIR)ca65 -t none $< -l $(LSTDIR)/$(basename $(notdir $<)).lst -o $(OBJDIR)/$(basename $(notdir $<)).o
	@rm -rf $@
	$(info Linking "$<" -> "$@" using $*.cfg)
	@$(CC65DIR)ld65 -C ./$(APPLEDIR)/$*.cfg $(OBJDIR)/$(basename $(notdir $<)).o -o $@ -m $(MAPDIR)/$(basename $(notdir $<)).map
	$(info "$@" done!)
	$(info )

$(BINDIR)/%.6502 : $(ASMDIR)/%.asm
	$(info )
	$(info Assembling "$<")
	@$(CC65DIR)ca65 -t none $< -l $(LSTDIR)/$(basename $(notdir $<)).lst -o $(OBJDIR)/$(basename $(notdir $<)).o
	@rm -rf $@
	$(info Linking "$<" -> "$@")
	@$(CC65DIR)ld65 -C custom.cfg $(OBJDIR)/$(basename $(notdir $<)).o -o $@ -m $(MAPDIR)/$(basename $(notdir $<)).map
	$(info "$@" done!)
	$(info )

$(LSTDIR) :
	@mkdir -p $@

$(OBJDIR) :
	@mkdir -p $@

$(BINDIR) :
	@mkdir -p $@
	
$(MAPDIR) :
	@mkdir -p $@

$(APPLEDIR) : $(APPLEDIR)/$(BINDIR) $(APPLEDIR)/$(LSTDIR) $(APPLEDIR)/$(OBJDIR) $(APPLEDIR)/$(MAPDIR)

$(APPLEDIR)/$(BINDIR) :
	@mkdir -p $@

$(APPLEDIR)/$(LSTDIR) :
	@mkdir -p $@

$(APPLEDIR)/$(OBJDIR) :
	@mkdir -p $@

$(APPLEDIR)/$(MAPDIR) :
	@mkdir -p $@

