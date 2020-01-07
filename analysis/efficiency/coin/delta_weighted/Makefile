CC=g++

ROOTFLAGS=`root-config --cflags --libs`

# my includes/libraries
EFF_DIR=/home/jmatter/ct_scripts/src/detector-efficiency
CTUTIL_DIR=/home/jmatter/ct_scripts/src/util
INC=$(EFF_DIR) $(CTUTIL_DIR)
INC_PARAMS=$(foreach d, $(INC), -I$d)

LIB=$(CTUTIL_DIR)/CTData.so
LIB+=$(CTUTIL_DIR)/CTCuts.so
LIB+=$(EFF_DIR)/Efficiency0D.so
LIB+=$(EFF_DIR)/Efficiency1D.so
LIB+=$(EFF_DIR)/Efficiency2D.so

CXXFLAGS=$(ROOTFLAGS) $(INC_PARAMS) $(LIB)

all: 
	$(CC) $(CXXFLAGS) -o calculate_all calculate_all.C
	$(CC) $(CXXFLAGS) -o calculate_all_by_run calculate_all_by_run.C
	$(CC) $(CXXFLAGS) -o calculate_quick calculate_quick.C
