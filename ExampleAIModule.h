#include <BWAPI.h>
#define UN using namespace
UN BWAPI;
UN Filter;
UN std;
using U = Unit;
using V = void;
auto&g = BroodwarPtr;
struct ExampleAIModule : AIModule {
	Unit hatchBuilder = NULL;
	Position enemy = Positions::Unknown;
	Player me = BWAPI::Broodwar->self();
	TilePosition selfstart = me->getStartLocation();
	bool hasPool = false;
	Unit scouter = NULL;
	bool eZerg = false;
	Race wace = Races::Unknown;
	int bases = 2;
	bool proxyHatch = false;
	TilePosition lastbuild = TilePositions::None;
	Position proxyPos = Positions::Unknown;
	vector<BWAPI::Position> scouts;
	vector<BWAPI::Unit> builders;
	vector<BWAPI::Unit> scouters;
	vector<BWAPI::Position> ordered;
	V onStart() {
		Broodwar->sendText("DSFGBSsdfgsdfg loaded successfully");
		Broodwar->enableFlag(Flag::UserInput);

		if (g->enemy()->getRace() == Races::Zerg) {
			eZerg = true;
		}
		else {
			eZerg = false;
		}

		if (g->mapHash() == "64e53c70a4036b42f6480f0fc9530b38789159c4") {
			bases = 2;
			Position region = g->getRegionAt(3776, 272)->getCenter();
			scouts.push_back(region);
			region = g->getRegionAt(3808, 3792)->getCenter();
			scouts.push_back(region);
			region = g->getRegionAt(320, 3792)->getCenter();
			scouts.push_back(region);
			region = g->getRegionAt(288, 272)->getCenter();
			scouts.push_back(region);
		}

		if (g->mapHash() == "97944269ea55365d13c310f46c9337f5e873dc6c") {
			bases = 1;
			Position region = g->getRegionAt(3808, 2160)->getCenter();
			scouts.push_back(region);
			region = g->getRegionAt(288, 912)->getCenter();
			scouts.push_back(region);
		}

		if (g->mapHash() == "614d0048c6cc9dcf08da1409462f22f2ac4f5a0") {
			bases = 1;
			Position region = g->getRegionAt(3296, 3824)->getCenter();
			scouts.push_back(region);
			region = g->getRegionAt(288, 272)->getCenter();
			scouts.push_back(region);
		}

		if (g->mapHash() == "d16719e736252d77fdbb0d8405f7879f564bfe56") {
			bases = 2;
			Position region = g->getRegionAt(3776, 1552)->getCenter();
			scouts.push_back(region);
			region = g->getRegionAt(320, 400)->getCenter();
			scouts.push_back(region);
			region = g->getRegionAt(1024, 3792)->getCenter();
			scouts.push_back(region);
		}

		if (g->mapHash() == "9bfc271360fa5bab3707a29e1326b84d0ff58911") {
			bases = 2;
			Position region = g->getRegionAt(288, 1456)->getCenter();
			scouts.push_back(region);
			region = g->getRegionAt(3808, 336)->getCenter();
			scouts.push_back(region);
			region = g->getRegionAt(3040, 3824)->getCenter();
			scouts.push_back(region);
		}

	}
	V onFrame() {
		g->drawTextScreen(0, 0, "Frame:%d", g->getFrameCount());

		if (enemy != Positions::None) {
			g->drawCircleMap(enemy, 50, BWAPI::Colors::Red, false);
		}

		for (auto &u : me->getUnits()) {

			if (scouters.size() < bases && ScoutersContains(u) == false && u->getType().isWorker()) {
				scouters.push_back(u);
			}

			if (u->getType().isWorker())
			{
				if (u->isIdle() && ScoutersContains(u) == false)
				{
					u->gather(u->getClosestUnit(IsMineralField));
				}

				if (ScoutersContains(u) && (u->isMoving() == false)) {
					if (enemy == Positions::Unknown) {
						for (auto &r : scouts) {
							if (g->isExplored(TilePosition{ r }) == false && OrderedContains(r) == false) {
								u->move(r);
								ordered.push_back(r);
								break;
							}
						}
					}
					else {
						if (me->minerals() <= 300 && u->isConstructing() == false) {
							Region region = g->getRegionAt(enemy)->getClosestAccessibleRegion();
							u->move(region->getCenter());
						}
					}
				}

				if (enemy != Positions::Unknown && me->minerals() >= 300 && wace != Races::Zerg && ScoutersContains(u) == true && proxyHatch == false) {
					if (u->isConstructing() == false) {
						if (u->isIdle() || u->isMoving()) {
							TilePosition build = g->getBuildLocation(UnitTypes::Zerg_Hatchery, TilePosition{ enemy }, 30, false);
							if (build != TilePositions::Invalid && alreadyBuilding(build) == false) {
								u->build(UnitTypes::Zerg_Hatchery, build);
								lastbuild = build;
							}
						}
					}
				}

				if (u->isMoving() == true && ScoutersContains(u) == true) {
					g->drawLineMap(u->getPosition(), u->getOrderTargetPosition(), BWAPI::Colors::Red);
				}


				// end of worker
			}

			if (u->getType() == UnitTypes::Zerg_Hatchery && shouldDrone() == true) {
				u->train(UnitTypes::Zerg_Drone);
			}

			if (builderscontains(u) == true) {

				if (u->isGatheringMinerals() && proxyPos != Positions::Unknown) {
					u->move(proxyPos);
				}

				if (me->minerals() > 75 && proxyPos != Positions::Unknown && u->isConstructing() == false && hasPool == true) {
					g->sendText("Trigger");
					if (g->hasCreep(TilePosition{ proxyPos }) == true) {
						g->sendText("has creep");
						TilePosition build = g->getBuildLocation(UnitTypes::Zerg_Creep_Colony, TilePosition{ proxyPos }, 30, true);
						if (build != TilePositions::Invalid) {
							g->sendText("build");
							u->build(UnitTypes::Zerg_Creep_Colony, build);
						}
					}
				}
			}

			if (u->getType() = UnitTypes::Zerg_Creep_Colony && me->minerals() > 75) {
				u->morph(UnitTypes::Zerg_Sunken_Colony);
			}

			// end of units loop
		}

		if (hasPool == false) {
			if (me->minerals() >= 200 && wace == Races::Zerg) {
				TilePosition build = g->getBuildLocation(UnitTypes::Zerg_Spawning_Pool, selfstart, 200, true);
				if (build != TilePositions::Invalid) {
					for (auto &u : me->getUnits()) {
						if (ScoutersContains(u) == false && u->getType().isWorker()) {
							u->build(UnitTypes::Zerg_Spawning_Pool, build);
							break;
						}
					}
				}
			}

			if (me->minerals() >= 200 && wace != Races::Zerg && proxyHatch == true) {
				TilePosition build = g->getBuildLocation(UnitTypes::Zerg_Spawning_Pool, selfstart, 200, true);
				if (build != TilePositions::Invalid) {
					for (auto &u : me->getUnits()) {
						if (ScoutersContains(u) == false && u->getType().isWorker() && builderscontains(u) == false) {
							u->build(UnitTypes::Zerg_Spawning_Pool, build);
							break;
						}
					}
				}
			}
		}



	}
	V onUnitShow(U u) {


		if (u->getPlayer() == g->enemy() && u->getType().isBuilding() == true && u->getPlayer()->getRace() == Races::Zerg) {
			if (enemy == Positions::Unknown) {
				g->sendText("Enemy Race is Zerg! No hatchery required");
			}
			wace = u->getPlayer()->getRace();
			enemy = u->getPosition();
			proxyPos = u->getPosition();
			if (scouters.size() != bases) {
				getBuilders(2);
			}
		}


		if (u->getPlayer() == g->enemy() && u->getType().isBuilding() == true && enemy == Positions::Unknown) {
			enemy = u->getPosition();
			wace = u->getPlayer()->getRace();
			g->sendText("ENEMY SPOTTED");
			for (auto &u : me->getUnits()) {
				if (ScoutersContains(u) == true) {
					u->move(enemy);
				}
			}
		}


	}

	V onUnitHide(U u) {}

	V onUnitCreate(U u) {


	}
	V onUnitDestroy(U u) {

		if (u->getType() == UnitTypes::Zerg_Spawning_Pool && hasPool == true) {
			hasPool = false;
		}

		if (ScoutersContains(u) == true) {
			scouter = NULL;
		}

		if (builderscontains(u) == true) {
			getBuilders(1);
		}


	}
	V onUnitMorph(U u) {


		if (u->getType() == UnitTypes::Zerg_Hatchery && u->getPlayer() == me && me->allUnitCount(UnitTypes::Zerg_Hatchery) > 1) {
			proxyPos = u->getPosition();
			proxyHatch = true;
			getBuilders(1);
		}


		if (u->getType() == UnitTypes::Zerg_Creep_Colony && u->getPlayer() == me) {
			getBuilders(1);
		}

		if (u->getType() == UnitTypes::Zerg_Spawning_Pool && u->getPlayer() == me) {
			hasPool = true;
			g->sendText("Pool is done");
		}


	}
	V onUnitRenegade(U u) {}
	V onUnitComplete(U u) {}

	bool builderscontains(Unit u) {

		if (builders.empty()) {
			return false;
		}

		if (std::find(builders.begin(), builders.end(), u) != builders.end()) {
			return true;
		}
		else {
			return false;
		}

	}

	bool ScoutersContains(Unit u) {

		if (scouters.empty()) {
			return false;
		}

		if (std::find(scouters.begin(), scouters.end(), u) != scouters.end()) {
			return true;
		}
		else {
			return false;
		}

	}

	bool OrderedContains(Position pos) {

		if (std::find(ordered.begin(), ordered.end(), pos) != ordered.end()) {
			return true;
		}
		else {
			return false;
		}

	}

	void getBuilders(int max) {
		int i = 1;
		for (auto &u : me->getUnits()) {
			if (u->getType().isWorker() && builderscontains(u) == false) {
				if (i <= max) {
					builders.push_back(u);
					i++;
				}


			}
		}

	}

	bool shouldDrone() {

		if (me->supplyUsed() <= 10) {
			return true;
		}

		if (enemy == Positions::Unknown) {
			return true;
		}

		return false;
	}


	bool alreadyBuilding(TilePosition here) {

		if (lastbuild == TilePositions::None) {
			return false;
		}

		if (lastbuild.getApproxDistance(here) > 5) {
			return false;
		}

		return true;
	}

	int getIndexInBuilders(Unit builder) {
		ptrdiff_t pos = find(builders.begin(), builders.end(), builder) - builders.begin();
		if (pos <= builders.max_size()) {
			return pos;
		}
	}


};

