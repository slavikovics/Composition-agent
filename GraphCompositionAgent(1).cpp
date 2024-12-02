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
      SC_LOG_ERROR("GraphCompositionAgent Error: Number of arguments must be equal to 2");
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

    list<Edge> firstGraphEdges = GetGraphsEdges(firstGraphEdgesAddreses);
    list<Edge> secondGraphEdges = GetGraphsEdges(secondGraphEdgesAddreses);
    int count = CountUniqueNodes(firstGraphNodesAddreses, secondGraphNodesAddreses);
    
    ScAddr* resultGraphNodes = new ScAddr[count];
    FindResultGraphNodes(firstGraphNodesAddreses, secondGraphNodesAddreses, resultGraphNodes, count);

    list<std::array<ScAddr, 2>> resultGraphEdges =  FindResultGraphEdges(resultGraphNodes, count, firstGraphEdges, secondGraphEdges);

    for (std::array<ScAddr, 2> edge : resultGraphEdges)
    {
      SC_LOG_INFO("Result edge: " << m_memoryCtx.HelperGetSystemIdtf(edge[0]) << " = " << m_memoryCtx.HelperGetSystemIdtf(edge[1]));
    }

    VisualizeAnswer(otherAddr, resultGraphEdges);
    
    delete[] resultGraphNodes;
    return SC_RESULT_OK;
  }

  void GraphCompositionAgent::VisualizeAnswer(ScAddr actionAddr, list<std::array<ScAddr, 2>> resultGraphEdges)
  {
    ScAddr resultStructNode = m_memoryCtx.CreateNode(ScType::NodeConstStruct);
    ScAddr edgeResult = m_memoryCtx.CreateEdge(ScType::EdgeDCommonConst, actionAddr, resultStructNode);
    m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, Keynodes::nrel_composition_result, edgeResult);
    for (std::array<ScAddr, 2> edge : resultGraphEdges)
    {
      m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, resultStructNode, edge[0]);
      m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, resultStructNode, edge[1]);
      ScAddr createdEdge = m_memoryCtx.CreateEdge(ScType::EdgeUCommonConst, edge[0], edge[1]);
      m_memoryCtx.CreateEdge(ScType::EdgeAccessConstPosPerm, resultStructNode, createdEdge);
    }
    m_memoryCtx.Save();
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

  list<GraphCompositionAgent::Edge> GraphCompositionAgent::GetGraphsEdges(list<ScAddr> edgesAddreses)
  {
    list<GraphCompositionAgent::Edge> edges;

    for (ScAddr edgeAddres : edgesAddreses)
    {
      ScIterator3Ptr const it3 = m_memoryCtx.Iterator3(
      ScType::NodeConst,
      edgeAddres,
      ScType::NodeConst 
      );

      it3->Next();

      edges.push_back({it3->Get(0), edgeAddres, it3->Get(2)});
    }

    return edges;
  }

  int GraphCompositionAgent::CountUniqueNodes(list<ScAddr> firstGraphNodes, list<ScAddr> secondGraphNodes)
  {
    int count = 0;
    for (ScAddr node : firstGraphNodes) count++;

    for (ScAddr firstNode : secondGraphNodes)
    {
      for (ScAddr secondNode : firstGraphNodes)
      {
        if (firstNode == secondNode)
        {
          count--;
          break;
        }
      }
      count++;
    }

    return count;
  }

  void GraphCompositionAgent::FindResultGraphNodes(list<ScAddr> firstGraphNodes, list<ScAddr> secondGraphNodes, ScAddr* resultGraphNodes, int count)
  {
    int i = 0;
    for (ScAddr node : firstGraphNodes)
    {
      resultGraphNodes[i] = node;
      i++;
    } 

    bool shouldBeAdded;

    for (ScAddr firstNode : secondGraphNodes)
    {
      shouldBeAdded = true;

      for (ScAddr secondNode : firstGraphNodes)
      {
        if (firstNode == secondNode)
        {
          shouldBeAdded = false;
          break;
        }
      }

      if (shouldBeAdded)
      {
          resultGraphNodes[i] = firstNode;
          i++;
      }
    }
  }

  list<std::array<ScAddr, 2>> GraphCompositionAgent::FindResultGraphEdges(ScAddr* resultGraphNodes, int count, list<GraphCompositionAgent::Edge> firstGraphEdges, list<GraphCompositionAgent::Edge> secondGraphEdges)
  {
    list<std::array<ScAddr, 2>> resultGraphEdges;

    for (int i = 0; i < count; i++)
    {
      for (int j = i; j < count; j++)
      {
        if (CheckIfEdgeShouldBeAddedToResult(resultGraphNodes[i], resultGraphNodes[j], firstGraphEdges, secondGraphEdges))
        {
          resultGraphEdges.push_back({resultGraphNodes[i], resultGraphNodes[j]});
        }
      }
    }

    return resultGraphEdges;
  }

  bool GraphCompositionAgent::CheckIfEdgeShouldBeAddedToResult(ScAddr firstNode, ScAddr secondNode, list<GraphCompositionAgent::Edge> firstGraphEdges, list<GraphCompositionAgent::Edge> secondGraphEdges)
  { 
    list<ScAddr> foundNodes;

    for (GraphCompositionAgent::Edge edge : firstGraphEdges)
    {
      if (edge[0] == firstNode)
      {
        foundNodes.push_back(edge[2]);
      }
      if (edge[2] == firstNode)
      {
        foundNodes.push_back(edge[0]);
      }
    }

    for (GraphCompositionAgent::Edge edge : secondGraphEdges)
    {
      for (ScAddr node : foundNodes)
      {
        if (edge[0] == node && edge[2] == secondNode)
        {
          return true;
        }
        if (edge[2] == node && edge[0] == secondNode)
        {
          return true;
        }
      }
    }

    return false;
  }

}  // namespace GraphCompositionModule
