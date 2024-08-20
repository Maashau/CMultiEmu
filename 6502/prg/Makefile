ASMDIR=./asm
OBJDIR=./obj
BINDIR=./bin
LSTDIR=./lst
MAPDIR=./map
CC65DIR=

all : $(OBJDIR) $(LSTDIR) $(BINDIR) $(MAPDIR) blink weekday kbTest memClear memWrite numQuery print

blink : $(BINDIR)/blink.6502
weekday : $(BINDIR)/weekday.6502
kbTest : $(BINDIR)/kbTest.6502
memClear : $(BINDIR)/memClear.6502
memWrite : $(BINDIR)/memWrite.6502
numQuery : $(BINDIR)/numQuery.6502
print : $(BINDIR)/print.6502

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
	@mkdir -p $(LSTDIR)

$(OBJDIR) :
	@mkdir -p $(OBJDIR)

$(BINDIR) :
	@mkdir -p $(BINDIR)
	
$(MAPDIR) :
	@mkdir -p $(MAPDIR)
