#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "GraphCompositionAgent.generated.hpp"

namespace exampleModule
{

class GraphCompositionAgent : public ScAgent
{

  SC_CLASS(Agent, Event(Keynodes::question_initiated, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

  typedef std::array<ScAddr, 3> Edge;

  std::list<ScAddr> GetElements(ScAddr firstElement, ScType secondElementType, ScType thirdElementType, int elementIndex);

  std::string GenerateGraphPreview(std::list<ScAddr> nodes);

  ScAddr GetArguments(ScAddr firstElement, ScType secondElementType, ScType thirdElementType, ScType fourthElementType, ScAddr fifthElement, int elementIndex);

  std::list<Edge> GetGraphsEdges(std::list<ScAddr> edgesAddreses);

  int CountUniqueNodes(std::list<ScAddr> firstGraphNodes, std::list<ScAddr> secondGraphNodes);

  void FindResultGraphNodes(std::list<ScAddr> firstGraphNodes, std::list<ScAddr> secondGraphNodes, ScAddr* resultGraphNodes, int count);

  std::list<std::array<ScAddr, 2>>  FindResultGraphEdges(ScAddr* resultGraphNodes, int count, std::list<Edge> firstGraphEdges, std::list<Edge> secondGraphEdges);

  bool CheckIfEdgeShouldBeAddedToResult(ScAddr firstNode, ScAddr secondNode, std::list<Edge> firstGraphEdges, std::list<Edge> secondGraphEdges);

  void VisualizeAnswer(ScAddr actionAddr, std::list<std::array<ScAddr, 2>> resultGraphEdges);
};

}  // namespace exampleModule
