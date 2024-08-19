ASMDIR=./asm
OBJDIR=./obj
BINDIR=./bin
LSTDIR=./lst
CC65DIR=

all : $(OBJDIR) $(LSTDIR) $(BINDIR) blink weekday kbTest memClear memWrite

blink : $(BINDIR)/blink.6502
weekday : $(BINDIR)/weekday.6502
kbTest : $(BINDIR)/kbTest.6502
memClear : $(BINDIR)/memClear.6502
memWrite : $(BINDIR)/memWrite.6502

$(BINDIR)/%.6502 : $(ASMDIR)/%.asm
	$(info )
	$(info Assembling "$<")
	@$(CC65DIR)ca65 -t none $< -l $(LSTDIR)/$(basename $(notdir $<)).lst -o $(OBJDIR)/$(basename $(notdir $<)).o
	@rm -rf $@
	$(info Linking "$<" -> "$@")
	@$(CC65DIR)cl65 -t none $(OBJDIR)/$(basename $(notdir $<)).o -o $@
	$(info "$@" done!)
	$(info )

$(LSTDIR) :
	@mkdir -p $(LSTDIR)

$(OBJDIR) :
	@mkdir -p $(OBJDIR)

$(BINDIR) :
	@mkdir -p $(BINDIR)
