#include "model.h"

int main(int argc, char** argv) {

	std::string config_file = argv[1];
	std::string prop_file = argv[2];

	boost::mpi::environment env(argc, argv);
	boost::mpi::communicator comm;

	repast::RepastProcess::init(config_file);
	
	{
		// Builds model
		PreyPredatorModel model(prop_file, argc, argv, &comm);

		// Init agents
		model.init();

		repast::ScheduleRunner& runner
			= repast::RepastProcess::instance()->getScheduleRunner();
		model.initSchedule(runner);

		runner.run();
	}

	repast::RepastProcess::instance()->done();
}
