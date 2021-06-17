#ifndef PP_OUTPUT_H
#define PP_OUTPUT_H

#include "repast_hpc/TDataSource.h"
#include "agent.h"
#include "types.h"

class PreyPredatorDataSource : public repast::TDataSource<int> {
	private:
		repast::SharedContext<PreyPredatorAgent>& context;
		AgentType type;

	protected:
		PreyPredatorDataSource(repast::SharedContext<PreyPredatorAgent>& context, AgentType type)
			: context(context), type(type) {
			}

		int getData() override;
};

struct PreyDataSource : public PreyPredatorDataSource {
	PreyDataSource(repast::SharedContext<PreyPredatorAgent>& context)
		: PreyPredatorDataSource(context, PREY) {
		}
};

struct PredatorDataSource : public PreyPredatorDataSource {
	PredatorDataSource(repast::SharedContext<PreyPredatorAgent>& context)
		: PreyPredatorDataSource(context, PREDATOR) {
		}
};

class GrassDataSource : public repast::TDataSource<int> {
	private:
		repast::SharedContext<Grass>& context;

	public:
		GrassDataSource(repast::SharedContext<Grass>& context)
			: context(context) {
			}

		int getData() override;
};
#endif
