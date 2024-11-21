#pragma once

#include <sc-memory/kpm/sc_agent.hpp>

#include "keynodes/keynodes.hpp"
#include "GraphCompositionAgent.generated.hpp"

namespace exampleModule
{
class GraphCompositionAgent : public ScAgent
{
  SC_CLASS(Agent, Event(Keynodes::action_find_argument, ScEvent::Type::AddOutputEdge))
  SC_GENERATED_BODY()

  std::list<ScAddr> GetElements(ScAddr firstElement, ScType secondElementType, ScType thirdElementType, int elementIndex);

  std::string GenerateGraphPreview(std::list<ScAddr> nodes);

  ScAddr GetArguments(ScAddr firstElement, ScType secondElementType, ScType thirdElementType, ScType fourthElementType, ScAddr fifthElement, int elementIndex);

  std::list<Edge*> GetEdges(ScAddr firstElement, ScType secondElementType, ScType thirdElementType, std::list<Node*> nodes);

  std::list<Node*> BuildNodesListFromScAddrs(std::list<ScAddr> scAddrs);
};

class Node
{
  private:

    void ParseIdentifier();

  public:

    bool operator==(const Node& node);

    ScAddr Addres;

    std::string RawIdentifier;

    std::string ParsedIdentifier;

    Node(ScAddr addres, std::string rawIdentifier);

    static Node* FindNodeByScAddr(ScAddr scAddr, std::list<Node*> nodes);

    Node();
};

class Edge
{
  public:

    ScAddr Addres;

    Node* FirstNode;

    Node* SecondNode;

    std::string RawIdentifier;

    std::string ParsedIdentifier;

    Edge(ScAddr addres, std::string rawIdentifier, Node* firstNode, Node* secondNode);

    Edge(ScAddr addres, Node* firstNode, Node* SecondNode);

    Edge(Node* firstNode, Node* secondNode);

    Edge();
};

class CompositionBuilder
{
  private:

    std::list<Node*> _firstGraphNodes;

    std::list<Edge*> _firstGraphEdges;

    std::list<Node*> _secondGraphNodes;

    std::list<Edge*> _secondGraphEdges;

    std::list<Node*> _resultGraphNodes;

    std::list<Edge*> _resultGraphEdges;

    Edge* FindEdge(std::list<Edge*> edges, Node* firstNode, Node* secondNode);

    bool HasEdge(std::list<Edge*> edges, Node* firstNode, Node* secondNode);

    bool AreConnectedByThirdNode(Node* firstNode, Node* secondNode);

    void FindAllResultGraphNodes();

    void FindAllResultGraphEdges();

  public:

    CompositionBuilder(std::list<Node*> firstGraphNodes, std::list<Edge*> firstGraphEdges, 
    std::list<Node*> secondGraphNodes, std::list<Edge*> secondGraphEdges);

    void BuildComposition();

    void LogCompositionResult();
};

}  // namespace exampleModule
