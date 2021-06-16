#include "repast_hpc/AgentId.h"

class Grass {
	private:
		repast::AgentId id;
		int _countdown;
		bool _grown;

	public:
		static int growing_rate;
		static const int type;

		Grass(repast::AgentId id, bool grown, int countdown)
			: id(id), _grown(grown), _countdown(countdown) {
			}

		repast::AgentId& getId() {
			return id;
		}

		const repast::AgentId& getId() const {
			return id;
		}

		bool grown() const {
			return _grown;
		}

		int countdown() const {
			return _countdown;
		}

		void reset() {
			_grown=false;
			_countdown = growing_rate;
		}

		void grow();
		void update(int current_rank, bool grown, int countdown);
};

struct GrassPackage {
	repast::AgentId id;
	int countdown;
	bool grown;

	GrassPackage() {};
	GrassPackage(const Grass& grass) :
		id(grass.getId()),
		countdown(grass.countdown()),
		grown(grass.grown()) {
		}

	template<typename Archive>
		void serialize(Archive& ar, const unsigned int) {
			ar & id;
			ar & countdown;
			ar & grown;
		}
};
