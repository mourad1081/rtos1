FLAGS= -std=c++11 -Wall

all: taskGenerator studyDM simDM

taskGenerator:
	g++ $(FLAGS) -DTASK_GENERATOR_MODULE *.cpp -o taskGenerator

studyDM:
	g++ $(FLAGS) -DSTUDY_DM_MODULE *.cpp -o studyDM

simDM:
	g++ $(FLAGS) -DSIM_DM_MODULE *.cpp -o simDM

clean:
	rm *.o
