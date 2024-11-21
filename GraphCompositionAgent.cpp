#include <iostream>

#include <sc-memory/sc_memory.hpp>
#include <sc-memory/sc_stream.hpp>
#include <sc-memory/sc_template_search.cpp>

#include <sc-agents-common/utils/IteratorUtils.hpp>
#include <sc-agents-common/utils/AgentUtils.hpp>

#include "GraphCompositionAgent.hpp"

using namespace std;
using namespace utils;

namespace exampleModule
{
  SC_AGENT_IMPLEMENTATION(GraphCompositionAgent)
  {
    SC_LOG_INFO("GraphCompositionAgent: agent started");

    list<ScAddr> argumentsList;
    argumentsList.push_back(GetArguments(otherAddr, ScType::EdgeAccessConstPosPerm, ScType::NodeConstStruct, ScType::EdgeAccessConstPosPerm, Keynodes::rrel_composition_argument_1, 2));
    argumentsList.push_back(GetArguments(otherAddr, ScType::EdgeAccessConstPosPerm, ScType::NodeConstStruct, ScType::EdgeAccessConstPosPerm, Keynodes::rrel_composition_argument_2, 2));

    if (argumentsList.size() != 2)
    {
      SC_LOG_ERROR("Error: Number of arguments must be equal to 2");
      return SC_RESULT_ERROR;
    }

    SC_LOG_INFO("GraphCompositionAgent: arguments accepted");

    bool argumentsOrder = false; // false for right, true for inverse

    ScAddr firstGraph = argumentsList.front();
    argumentsList.pop_front();

    list<ScAddr> firstGraphNodesAddreses = GetElements(firstGraph, ScType::EdgeAccessConstPosPerm, ScType::NodeConst, 2);
    SC_LOG_DEBUG("GraphCompositionAgent: first graph contains " << firstGraphNodesAddreses.size() << " nodes.");

    list<ScAddr> firstGraphEdgesAddreses = GetElements(firstGraph, ScType::EdgeAccessConstPosPerm, ScType::EdgeUCommonConst, 2);
    SC_LOG_DEBUG("GraphCompositionAgent: first graph contains " << firstGraphEdgesAddreses.size() << " edges.");


    // working with graph2
    ScAddr secondGraph = argumentsList.front();
    argumentsList.pop_front();

    list<ScAddr> secondGraphNodesAddreses = GetElements(secondGraph, ScType::EdgeAccessConstPosPerm, ScType::NodeConst, 2);
    SC_LOG_DEBUG("GraphCompositionAgent: second graph contains " << secondGraphNodesAddreses.size() << " nodes.");

    list<ScAddr> secondGraphEdgesAddreses = GetElements(secondGraph, ScType::EdgeAccessConstPosPerm, ScType::EdgeUCommonConst, 2);
    SC_LOG_DEBUG("GraphCompositionAgent: second graph contains " << secondGraphEdgesAddreses.size() << " edges.");

    std::string firstGraphPreview = GenerateGraphPreview(firstGraphNodesAddreses);
    std::string secondGraphPreview = GenerateGraphPreview(secondGraphNodesAddreses);

    SC_LOG_INFO("First graph preview: \n" << firstGraphPreview);
    SC_LOG_INFO("Second graph preview: \n" << secondGraphPreview);

    // Logging nodes identifiers
    // ? ScMemoryContext(ScMemoryContext const & other) = delete;

    SC_LOG_INFO("Graph " << "first graph" << " nodes: ");
    for(ScAddr node : firstGraphNodesAddreses)
    {
      SC_LOG_INFO(m_memoryCtx.HelperGetSystemIdtf(node));
    }

    SC_LOG_INFO("Graph " << "second graph" << " nodes: ");
    for(ScAddr node : secondGraphNodesAddreses)
    {
      SC_LOG_INFO(m_memoryCtx.HelperGetSystemIdtf(node));
    }

    std::list<Node*> firstGraphNodes = BuildNodesListFromScAddrs(firstGraphNodesAddreses);
    std::list<Node*> secondGraphNodes = BuildNodesListFromScAddrs(secondGraphNodesAddreses);

    std::list<Edge*> firstGraphEdges = GetEdges(firstGraph, ScType::EdgeAccessConstPosPerm, ScType::NodeConst, firstGraphNodes);
    std::list<Edge*> secondGraphEdges = GetEdges(secondGraph, ScType::EdgeAccessConstPosPerm, ScType::NodeConst, secondGraphNodes);

    CompositionBuilder compositionBuilder = CompositionBuilder(firstGraphNodes,
                                                               firstGraphEdges,
                                                               secondGraphNodes,
                                                               secondGraphEdges
                                                              );
    compositionBuilder.BuildComposition();

    return SC_RESULT_OK;
  }

  ScAddr GraphCompositionAgent::GetArguments(ScAddr firstElement, ScType secondElementType, ScType thirdElementType, ScType fourthElementType, ScAddr fifthElement, int elementIndex)
  {
    ScIterator5Ptr const argumentsIterator = m_memoryCtx.Iterator5(
      firstElement,
      secondElementType,
      thirdElementType,
      fourthElementType,
      fifthElement
    );

    argumentsIterator->Next();
    return argumentsIterator->Get(elementIndex);
  }

  std::list<ScAddr> GraphCompositionAgent::GetElements(ScAddr firstElement, ScType secondElementType, ScType thirdElementType, int elementIndex)
  {
    list<ScAddr> result;

    ScIterator3Ptr const it3 = m_memoryCtx.Iterator3(
      firstElement,
      secondElementType,
      thirdElementType 
    );

    while(it3->Next())
    {
      result.push_back(it3->Get(elementIndex));
    }

    return result;
  }

  std::list<Edge*> GraphCompositionAgent::GetEdges(ScAddr firstElement, ScType secondElementType, ScType thirdElementType, std::list<Node*> nodes)
  {
    std::list<Edge*> result;

    ScIterator3Ptr const it3 = m_memoryCtx.Iterator3(
      firstElement,
      secondElementType,
      thirdElementType 
    );

    while(it3->Next())
    {
      Edge* edge = new Edge(it3->Get(1), 
                       Node::FindNodeByScAddr(it3->Get(0), nodes), 
                       Node::FindNodeByScAddr(it3->Get(2), nodes));
      result.push_back(edge);
    }

    return result;
  }

  Node* Node::FindNodeByScAddr(ScAddr scAddr, std::list<Node*> nodes)
  {
    for (Node* node : nodes)
    {
      if (node->Addres == scAddr) return node;
    }

    SC_LOG_ERROR("Node not found");
    throw exception();
  }

  std::list<Node*> GraphCompositionAgent::BuildNodesListFromScAddrs(std::list<ScAddr> scAddrs)
  {   
    std::list<Node*> nodes;

    for (ScAddr scAddr : scAddrs)
    {
      Node* node = new Node(scAddr, m_memoryCtx.HelperGetSystemIdtf(scAddr)); 
      nodes.push_back(node);
    }

    return nodes;
  }

  std::string GraphCompositionAgent::GenerateGraphPreview(std::list<ScAddr> nodes)
  {
    string responce;

    int nodesCount = nodes.size();
    ScAddr* nodesScAddreses = new ScAddr[nodesCount];
    for (int i = 0; i < nodesCount; i++)
    {
      nodesScAddreses[i] = nodes.front();
      nodes.pop_front();
    }

    for (int i = 0; i < nodesCount; i++)
    {
      std::list<ScAddr> connectedNodes = GetElements(nodesScAddreses[i], ScType::EdgeUCommonConst, ScType::NodeConst, 2);
      for (ScAddr node : connectedNodes)
      {
        int j;
        for (j = 0; j < nodesCount; j++)
        {
          if (nodesScAddreses[j] == node) break;
        }
        if (j >= i)
        {
          responce += std::to_string(i) + "=" + std::to_string(j) + "\n";
        }
      }  
    }

    delete[] nodesScAddreses;    
    return responce;
  }

  Node::Node(ScAddr addres, std::string rawIdentifier)
  {
    RawIdentifier = rawIdentifier;
    Addres = addres;
    ParseIdentifier();
  }

  void Node::ParseIdentifier()
  {
    int i = RawIdentifier.find('_');
    ParsedIdentifier = RawIdentifier.substr(0, i);
  }

  bool Node::operator==(const Node& node)
  {
    if (node.Addres == this->Addres) return true;
    return false;
  }

  Edge::Edge(ScAddr addres, std::string rawIdentifier, Node* firstNode, Node* secondNode)
  {
    Addres = addres;
    RawIdentifier = rawIdentifier;
    FirstNode = firstNode;
    SecondNode = secondNode;
  }

  Edge::Edge(ScAddr addres, Node* firstNode, Node* secondNode)
  {
    Addres = addres;
    FirstNode = firstNode;
    SecondNode = secondNode;
  }

  CompositionBuilder::CompositionBuilder(std::list<Node*> firstGraphNodes, std::list<Edge*> firstGraphEdges, 
  std::list<Node*> secondGraphNodes, std::list<Edge*> secondGraphEdges)
  {
    _firstGraphNodes = firstGraphNodes;
    _firstGraphEdges = firstGraphEdges;
    _secondGraphNodes = secondGraphNodes;
    _secondGraphEdges = secondGraphEdges;
  }

  Edge* CompositionBuilder::FindEdge(std::list<Edge*> edges, Node* firstNode, Node* secondNode)
  {
    for(Edge* edge : edges)
    {
      if ((edge->FirstNode == firstNode && edge->SecondNode == secondNode) || 
      (edge->FirstNode == secondNode && edge->SecondNode == firstNode))
      {
        return edge;
      }
    }

    SC_LOG_ERROR("Edge not found");
    throw exception();
  }

  bool CompositionBuilder::HasEdge(std::list<Edge*> edges, Node* firstNode, Node* secondNode)
  {
    for(Edge* edge : edges)
    {
      if ((edge->FirstNode == firstNode && edge->SecondNode == secondNode) || 
      (edge->FirstNode == secondNode && edge->SecondNode == firstNode))
      {
        return true;
      }
    }

    return false;
  }

  bool CompositionBuilder::AreConnectedByThirdNode(Node* firstNode, Node* secondNode)
  { 
    std::list<Edge*> edgesForFirstNode;
    for(Edge* edge : _firstGraphEdges)
    {
      if(edge->FirstNode == firstNode || edge->SecondNode == firstNode)
      {
        edgesForFirstNode.push_back(edge);
      }
    }

    for (Edge* edge : edgesForFirstNode)
    {
      if (edge->FirstNode == secondNode || edge->SecondNode == firstNode)
      {
        return true;
      }
    }
  }

  void CompositionBuilder::FindAllResultGraphNodes()
  {
    for (Node* node : _firstGraphNodes)
    {
      _resultGraphNodes.push_back(node);
    }

    bool shouldBeAdded = true;

    for (Node* node : _secondGraphNodes)
    {
      for (Node* resultNode : _resultGraphNodes)
      {
        if (node == resultNode)
        {
          shouldBeAdded = false;
          break;
        }
      }
      if (shouldBeAdded)
      {
        _resultGraphNodes.push_back(node);
      }
      shouldBeAdded = true;
    }
  }

  void CompositionBuilder::FindAllResultGraphEdges()
  {
    for (Node* firstNode : _resultGraphNodes)
    {
      for (Node* secondNode : _resultGraphNodes)
      {
        if (HasEdge(_resultGraphEdges, firstNode, secondNode)) continue;
        if (AreConnectedByThirdNode(firstNode, secondNode))
        {
          Edge* edge = new Edge(firstNode, secondNode);
          _resultGraphEdges.push_back(edge);
        }
      }
    }
  }

  void CompositionBuilder::BuildComposition()
  {
    FindAllResultGraphNodes();
    FindAllResultGraphEdges();
  }

  void CompositionBuilder::LogCompositionResult()
  {
    
  }

}  // namespace GraphCompositionModule
