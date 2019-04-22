/* 3, 201828018629015, Hongliang Pan */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <vector>
#include "GraphLite.h"

#define VERTEX_CLASS_NAME(name) DirectedTriangleCount##name
//Result of each node
typedef struct NodeResult{
    int num_in = 0;
    int num_out = 0;
    int num_through = 0;
    int num_cycle = 0;
}NodeResult;
//Message
typedef struct Message{
    int NodeId = -1;
    int check = 0; //1 represents for self node, 2 represents for neighbour node
    int mark1 = 0; //1 message from self node to out neighbour, -1 means to in neighbour
    int mark2 = 0; //1 message from neighbour node to out neighbour, -1 means to in neighbour
}Message;
NodeResult nodeResult;
class VERTEX_CLASS_NAME(InputFormatter): public InputFormatter {
public:
    int64_t getVertexNum() {
        unsigned long long n;
        sscanf(m_ptotal_vertex_line, "%lld", &n);
        m_total_vertex= n;
        return m_total_vertex;
    }
    int64_t getEdgeNum() {
        unsigned long long n;
        sscanf(m_ptotal_edge_line, "%lld", &n);
        m_total_edge= n;
        return m_total_edge;
    }
    int getVertexValueSize() {
        m_n_value_size = sizeof(NodeResult);
        return m_n_value_size;
    }
    int getEdgeValueSize() {
        m_e_value_size = sizeof(double);
        return m_e_value_size;
    }
    int getMessageValueSize() {
        m_m_value_size = sizeof(Message);
        return m_m_value_size;
    }
    void loadGraph() {
        unsigned long long last_vertex;
        unsigned long long from;
        unsigned long long to;
        double weight = 0;
        int outdegree = 0;

        NodeResult value;
        value.num_in = 0;
        value.num_out = 0;
        value.num_through = 0;
        value.num_cycle = 0;
        
        const char *line= getEdgeLine();

        // Note: modify this if an edge weight is to be read
        //       modify the 'weight' variable

        sscanf(line, "%lld %lld", &from, &to);
        addEdge(from, to, &weight);

        last_vertex = from;
        ++outdegree;
        for (int64_t i = 1; i < m_total_edge; ++i) {
            line= getEdgeLine();

            // Note: modify this if an edge weight is to be read
            //       modify the 'weight' variable

            sscanf(line, "%lld %lld", &from, &to);
            if (last_vertex != from) {
                addVertex(last_vertex, &value, outdegree);
                last_vertex = from;
                outdegree = 1;
            } else {
                ++outdegree;
            }
            addEdge(from, to, &weight);
        }
        addVertex(last_vertex, &value, outdegree);
    }
};

class VERTEX_CLASS_NAME(OutputFormatter): public OutputFormatter {
public:
    void writeResult() {
        char s1[1024];
        char s2[1024];
        char s3[1024];
        char s4[1024];
        int n1 = sprintf(s1, "in: %d\n", nodeResult.num_in);
        writeNextResLine(s1, n1);
        int n2 = sprintf(s2, "out: %d\n", nodeResult.num_out);
        writeNextResLine(s2, n2);
        int n3 = sprintf(s3, "through: %d\n", nodeResult.num_through);
        writeNextResLine(s3, n3);
        int n4 = sprintf(s4, "cycle: %d\n", nodeResult.num_cycle);
        writeNextResLine(s4, n4);
    }
};

// An aggregator that records a NodeResult value to compute sum
class VERTEX_CLASS_NAME(Aggregator): public Aggregator<NodeResult> {
public:
    void init() {
    }
    void* getGlobal() {
        return &m_global;
    }
    void setGlobal(const void* p) {
        m_global = * (NodeResult *)p;
    }
    void* getLocal() {
        return &m_local;
    }
    void merge(const void* p) {
        m_global.num_in += (* (NodeResult *)p).num_in;
        m_global.num_out += (* (NodeResult *)p).num_out;
        m_global.num_through += (* (NodeResult *)p).num_through;
        m_global.num_cycle += (* (NodeResult *)p).num_cycle;
    }
    void accumulate(const void* p) {
        m_local.num_in += (* (NodeResult *)p).num_in;
        m_local.num_out += (* (NodeResult *)p).num_out;
        m_local.num_through += (* (NodeResult *)p).num_through;
        m_local.num_cycle += (* (NodeResult *)p).num_cycle;
    }
};

class VERTEX_CLASS_NAME(): public Vertex <NodeResult, double, Message> {
public:
    void compute(MessageIterator* pmsgs) {
        // step 1: send self id to all out neighbours
    	if (getSuperstep() == 0){
    		Message mes;
    		mes.NodeId = getVertexId();
    		mes.check = 1;
    		mes.mark1 = 1;
    		sendMessageToAllNeighbors(mes);
    	}
    	//step 2: 
    	//a. save in neighbours ids and out neighbours ids in vector<int>vecin, vecout
    	//b. vector<Message> vecmes save all neighbours messages
    	//c. send self id to all neighbours
    	//d. send all neighbour ids to all neighbours

     	else if(getSuperstep()==1){
     		std::vector<int> vecin;
     		std::vector<int> vecout;
     		std::vector<Message> vecmes;
     		//a. save in/out neighbour ids and messages
     		//b. save out neighbour ids and messages
     		for ( ; ! pmsgs->done(); pmsgs->next() ) {
                Message temp = pmsgs->getValue();
                vecin.push_back(temp.NodeId);
                vecmes.push_back(temp);
            }
            for(OutEdgeIterator it = getOutEdgeIterator();!it.done();it.next()){
            	int id = it.target();
            	vecout.push_back(id);
            	Message mes;
            	mes.NodeId = id;
            	mes.check = 1;
            	mes.mark1 = -1;
            	vecmes.push_back(mes);
            }

            //c. send self id to all neighbours
            //c1. send to all out neighbour
            Message mes;
            mes.NodeId = getVertexId();
            mes.check = 1;
            mes.mark1 = 1;
            sendMessageToAllNeighbors(mes);

            //c2. send to all in neighbour
            for(int i=0;i<vecin.size();i++){
            	Message mes;
            	mes.NodeId = getVertexId();
            	mes.check = 1;
            	mes.mark1 = -1;
            	sendMessageTo(vecin[i],mes);
            }


            //d. send all neighbours to all neighbours
            //d1. send neighbours to in neighbours
            for(int i=0;i<vecin.size();i++){
            	for(int j=0;j<vecmes.size();j++){
            		Message mes;
            		mes.NodeId = vecmes[j].NodeId;
            		mes.check = 2;
            		mes.mark2 = -1;
            		mes.mark1 = vecmes[j].mark1;
            		if(vecin[i]!=mes.NodeId){
            			sendMessageTo(vecin[i],mes);
            		}
            	}
            }
            //d2. send neoghbour to out neighbours
            for(int i=0;i<vecout.size();i++){
            	for(int j=0;j<vecmes.size();j++){
            		Message mes;
            		mes.NodeId = vecmes[j].NodeId;
            		mes.check = 2;
            		mes.mark2 = 1;
            		mes.mark1 = vecmes[j].mark1;
            		if(vecout[i]!=mes.NodeId){
            			sendMessageTo(vecout[i],mes);
            		}
            	}
            }
    	}
    	//step 3:
    	//a. save all neighbours into vector n
    	//b. save all neighbours of neighbours into vector nn
    	//c. if n.id == nn.id shows there exists a triangle.
    	//d. add num to nodeResult
    	else if(getSuperstep()==2){
    		std::vector<Message> n;
    		std::vector<Message> nn;
    		for ( ; ! pmsgs->done(); pmsgs->next() ) {
                Message temp = pmsgs->getValue();
                if(temp.check == 1){
                	n.push_back(temp);
                }else if(temp.check == 2){
                	nn.push_back(temp);
                }
            }
            NodeResult tempNodeResult;
            for(int i=0;i<n.size();i++){
            	for(int j=0;j<nn.size();j++){
            		if(n[i].NodeId == nn[j].NodeId){
            			if(n[i].mark1 == 1){
            				if(nn[j].mark2 == 1){
            					tempNodeResult.num_in+=1;
            				}
            				else if(nn[j].mark2 == -1){
            					if(nn[j].mark1 == -1){
            						tempNodeResult.num_cycle+=1;
            					}else if(nn[j].mark1 == 1){
            						tempNodeResult.num_through+=1;
            					}
            				}
            			}
            			else if(n[i].mark1 == -1){
            				if(nn[j].mark2 == -1){
            					tempNodeResult.num_out+=1;
            				}
            				else if(nn[j].mark2 == 1){
            					if(nn[j].mark1 == 1){
            						tempNodeResult.num_cycle+=1;
            					}else if(nn[j].mark1 == -1){
            						tempNodeResult.num_through+=1;
            					}
            				}
            				
            			}
            		}
            	}
            }
    	
            tempNodeResult.num_in = tempNodeResult.num_in/2;
            tempNodeResult.num_out = tempNodeResult.num_out/2;
            tempNodeResult.num_through = tempNodeResult.num_through/2;
            tempNodeResult.num_cycle = tempNodeResult.num_cycle/2;
            * mutableValue() = tempNodeResult;
            accumulateAggr(0,&tempNodeResult);
        }
       	else if(getSuperstep()==3){
       		nodeResult = * (NodeResult *)getAggrGlobal(0);
       		voteToHalt(); 
       		return;
       	}
    }
};

class VERTEX_CLASS_NAME(Graph): public Graph {
public:
    VERTEX_CLASS_NAME(Aggregator)* aggregator;

public:
    // argv[0]: PageRankVertex.so
    // argv[1]: <input path>
    // argv[2]: <output path>
    void init(int argc, char* argv[]) {

        setNumHosts(5);
        setHost(0, "localhost", 1411);
        setHost(1, "localhost", 1421);
        setHost(2, "localhost", 1431);
        setHost(3, "localhost", 1441);
        setHost(4, "localhost", 1451);

        if (argc < 3) {
           printf ("Usage: %s <input path> <output path>\n", argv[0]);
           exit(1);
        }

        m_pin_path = argv[1];
        m_pout_path = argv[2];

        aggregator = new VERTEX_CLASS_NAME(Aggregator)[1];
        regNumAggr(1);
        regAggr(0, &aggregator[0]);
    }

    void term() {
        delete[] aggregator;
    }
};

/* STOP: do not change the code below. */
extern "C" Graph* create_graph() {
    Graph* pgraph = new VERTEX_CLASS_NAME(Graph);

    pgraph->m_pin_formatter = new VERTEX_CLASS_NAME(InputFormatter);
    pgraph->m_pout_formatter = new VERTEX_CLASS_NAME(OutputFormatter);
    pgraph->m_pver_base = new VERTEX_CLASS_NAME();

    return pgraph;
}

extern "C" void destroy_graph(Graph* pobject) {
    delete ( VERTEX_CLASS_NAME()* )(pobject->m_pver_base);
    delete ( VERTEX_CLASS_NAME(OutputFormatter)* )(pobject->m_pout_formatter);
    delete ( VERTEX_CLASS_NAME(InputFormatter)* )(pobject->m_pin_formatter);
    delete ( VERTEX_CLASS_NAME(Graph)* )pobject;
}