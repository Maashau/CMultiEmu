ASMDIR=./asm
OBJDIR=./obj
BINDIR=./bin
LSTDIR=./lst
MAPDIR=./map
CC65DIR=

all : directories appleIEmu

directories : $(OBJDIR) $(LSTDIR) $(BINDIR) $(MAPDIR)

appleIEmu : intBasic wozmon
intBasic : $(BINDIR)/intBasic.rom
wozmon : $(BINDIR)/wozmon.rom

$(BINDIR)/%.rom : $(ASMDIR)/%.asm
	$(info )
	$(info Assembling "$<")
	@$(CC65DIR)ca65 -t none $< -l $(LSTDIR)/$(basename $(notdir $<)).lst -o $(OBJDIR)/$(basename $(notdir $<)).o
	@rm -rf $@
	@$(CC65DIR)ld65 -C $(ASMDIR)/$*.cfg $(OBJDIR)/$(basename $(notdir $<)).o -o $@ -m $(MAPDIR)/$(basename $(notdir $<)).map

$(LSTDIR) :
	@mkdir -p $@

$(OBJDIR) :
	@mkdir -p $@

$(BINDIR) :
	@mkdir -p $@
	
$(MAPDIR) :
	@mkdir -p $@
