CC = gcc -Wall 
FLAGS = -c 
CFLAG = $(patsubst %,-I%,$(IDIR)) 
LIBS = -lm -lpthread
EXECUTABLE = MinGraphPath

IDIR = $(wildcard ./Headers/*)
ODIR = ./Obj
SRCDIR =./Src
RESDIR = $(wildcard ./Results/*/*)



SRC_SUB_DIR_COMPONENT = $(SRCDIR)/Components
SRC_SUB_DIR_EXCLUSIVE_STRUCTS = $(SRCDIR)/ExclusiveStructs
SRC_SUB_DIR_GENERIC_STRUCTS = $(SRCDIR)/GenericStructs
SRC_SUB_DIR_GRAPH = $(SRCDIR)/Graph
SRC_SUB_DIR_HYPERGRAPH = $(SRCDIR)/HyperGraph
SRC_SUB_DIR_INDEXES = $(SRCDIR)/Indexes
SRC_SUB_DIR_THREADS = $(SRCDIR)/Threads
SRC_SUB_DIR_UTILITY_FUNCTIONS = $(SRCDIR)/UtilityFuctions
SRC_SUB_DIR_MAIN = $(SRCDIR)/Main

_OBJ = stronglyConnectedComponents.o weaklyConnectedConponens.o queue.o stack.o gQueue.o gStack.o ghash.o glist.o buffer.o graph.o \
graphQA.o hyperGraph.o grail.o nodeIndex.o main.o jobScheduler.o compareResults.o datasetReader.o
 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


all: $(OBJ) 
	$(CC) -o $(EXECUTABLE) $^ $(CFLAG) $(LIBS)
	

$(ODIR)/%.o: $(SRC_SUB_DIR_COMPONENT)/%.c 
	$(CC) -c $(CFLAG) $< -o $@  

$(ODIR)/%.o: $(SRC_SUB_DIR_EXCLUSIVE_STRUCTS)/%.c 
	$(CC) -c $(CFLAG) $< -o $@  

$(ODIR)/%.o: $(SRC_SUB_DIR_GENERIC_STRUCTS)/%.c 
	$(CC) -c $(CFLAG) $< -o $@  

$(ODIR)/%.o: $(SRC_SUB_DIR_GRAPH)/%.c 
	$(CC) -c $(CFLAG) $< -o $@  

$(ODIR)/%.o: $(SRC_SUB_DIR_HYPERGRAPH)/%.c 
	$(CC) -c $(CFLAG) $< -o $@  

$(ODIR)/%.o: $(SRC_SUB_DIR_INDEXES)/%.c 
	$(CC) -c $(CFLAG) $< -o $@  

$(ODIR)/%.o: $(SRC_SUB_DIR_THREADS)/%.c 
	$(CC) -c $(CFLAG) $< -o $@  

$(ODIR)/%.o: $(SRC_SUB_DIR_UTILITY_FUNCTIONS)/%.c 
	$(CC) -c $(CFLAG) $< -o $@  

$(ODIR)/%.o: $(SRC_SUB_DIR_MAIN)/%.c 
	$(CC) -c $(CFLAG) $< -o $@  

.PHONY: clean

clean:
	rm -rf $(ODIR)/*.o $(EXECUTABLE) $(RESDIR)
