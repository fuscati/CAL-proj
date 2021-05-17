#include <unistd.h>
#include "Map.h"

#define DIST_TO_KM 0.00000001
City_Map read_file(string filename);
void print_gui(City_Map city_Map);
City_Map read_folder(string folder);
void read_edges(string filename, City_Map &city_map);
void read_nodes(string filename, City_Map &city_map);
void read_tags(string filename, City_Map &city_map, string type);
double distance(node_data v1, node_data v2);

int main() {

    string city;
    //printf("Enter the city to load the map from: ");
    //cin>>city;
    City_Map city_map;

    city_map=read_folder("Gondomar");

    city_map.garagem=1199479655;

    //city_map.dest=663689378;
    //city_map.dest=1198840042;
    city_map.loja=1222473635;

    city_map.graph.bfs(city_map.vertexes.at(city_map.garagem));

    city_map.remove_non_visited();
    city_map.fill_encomendas();
    city_map.plan_routes();
/*    double dist=city_map.graph.aStar(city_map.vertexes.at(city_map.garagem),city_map.vertexes.at(city_map.dest));
    printf("dist: %.2lf km\n",dist*DIST_TO_KM);*/
    print_gui(city_map);

    return 0;
}

City_Map read_folder(string city)
{
    City_Map city_map;
    string original = city;
    city[0] = city[0] + 32;

    string nodes_file = "../src/maps/PortugalMaps/" + original + "/nodes_x_y_" + city + ".txt";
    string edges_file = "../src/maps/PortugalMaps/" + original + "/edges_" + city + ".txt";
    string houses_file = "../src/maps/TagExamples/" + original + "/t10_tags_" + city + ".txt";
    string stores_file = "../src/maps/TagExamples/" + original + "/t04_tags_" + city + ".txt";
    string rechargers_file = "../src/maps/TagExamples/" + original + "/t08_tags_" + city + ".txt";

    read_nodes(nodes_file,city_map);
    read_edges(edges_file,city_map);
    read_tags(houses_file, city_map, "casa");
    read_tags(stores_file, city_map, "loja");
    read_tags(rechargers_file, city_map, "carregador");


    return city_map;
}

void read_edges(string filename, City_Map &city_map)
{
    fstream file;
    double weight;
    int n_edges;
    unsigned long  v1,v2;
    file.open(filename,ios::in);  // open a file to perform write operation using file object

    if (file.is_open()) {   //checking whether the file is open
        string line;

        getline(file, line);

        sscanf(line.c_str(), "%d", &n_edges);

        for (int i=0;i<n_edges;i++){
            getline(file, line);
            sscanf(line.c_str(),"(%lu, %lu)",&v1,&v2);

            weight=distance(city_map.vertexes.at(v1)->info,city_map.vertexes.at(v2)->info);

            city_map.vertexes.at(v1)->addEdge(city_map.vertexes.at(v2),v2,weight);
            city_map.vertexes.at(v2)->addEdge(city_map.vertexes.at(v1),v1,weight);

        }

    }
    else{
        printf("Error opening edges file\n");
        exit(1);
    }
    file.close();
}

void read_nodes(string filename, City_Map &city_map)
{
    fstream file;
    int n_nodes;
    unsigned long id;
    double x,y;
    Vertex *v;
    file.open(filename,ios::in);  // open a file to perform write operation using file object

    if (file.is_open()) {   //checking whether the file is open
        string line;

        getline(file, line);
        sscanf(line.c_str(), "%d", &n_nodes);
        for (int i = 0; i < n_nodes; i++) {
            getline(file, line);
            sscanf(line.c_str(), "(%lu, %lf, %lf)", &id, &x, &y);

            v=new Vertex(x, y);
            v->info.id=id;
            city_map.vertexes.insert({id,v});
            city_map.graph.addVertex(v);
        }
    }
    else{
        printf("That city does not exist\n");
        exit(1);
    }
    file.close();
}

void read_tags(string filename, City_Map &city_map, string type) {
    fstream file;
    int n_tags, n_nodes;
    unsigned long id;
    Vertex *v;
    file.open(filename,ios::in);  // open a file to perform write operation using file object

    if (file.is_open()) {   //checking whether the file is open
        string line;

        getline(file, line);
        sscanf(line.c_str(), "%d", &n_tags);


        for (int i = 0; i < n_tags; i++) {
            getline(file, line);
            getline(file, line);
            sscanf(line.c_str(), "%d", &n_nodes);

            for (int j = 0; j < n_nodes; j++){
                getline(file, line);
                sscanf(line.c_str(), "%lu", &id);


                if(type == "casa") {
                    city_map.casas.push_back(id);
                    city_map.vertexes.at(id)->info.is_casa = true;
                }

                else if(type == "loja") {
                    city_map.lojas.push_back(id);
                    city_map.vertexes.at(id)->info.is_loja = true;
                }

                else {
                    city_map.carregadores.push_back(id);
                    city_map.vertexes.at(id)->info.is_carregador = true;
                }
            }

        }
    }
    else{
        printf("Error opening tags file\n");
        exit(1);
    }
    file.close();
}


double distance(node_data v1, node_data v2)
{
    return(sqrt(((v1.x+v2.x)*(v1.x+v2.x))+((v1.y+v2.y)*(v1.y+v2.y))));
}

void print_gui(City_Map city_map){
    unsigned long garagem=city_map.garagem;
    Graph graph=city_map.graph;
    GraphViewer gv;

    // Set coordinates of window center
    map<unsigned long, Vertex*>::iterator it=city_map.vertexes.begin();
    gv.setCenter(sf::Vector2f(city_map.vertexes.at(city_map.garagem)->info.x, city_map.vertexes.at(city_map.garagem)->info.y));

    // Create window
    while (it!=city_map.vertexes.end())
    {
        node_data data=it->second->info;
        GraphViewer::Node &node = gv.addNode(it->first,sf::Vector2f(data.x,data.y));
        node.setColor(GraphViewer::BLACK);

/*
            if(data.is_carregador )
                node.setColor(GraphViewer::YELLOW);
            if(data.is_loja)
                node.setColor(GraphViewer::CYAN);
            if(data.is_casa)
                node.setColor(GraphViewer::RED);
            node.setLabel(to_string(it->first));
*/
        if(it->first==garagem)
            node.setColor(GraphViewer::GREEN);


        it++;
        //node.setSize(0.0001);
        //node.setOutlineThickness(0.1);
    }

    int k=0;
    it=city_map.vertexes.begin();
    while (it!=city_map.vertexes.end()) {

        for (int j=0;j<it->second->adj.size();j++)
        {
            GraphViewer::Edge &ed = gv.addEdge(k,gv.getNode(it->first),gv.getNode(it->second->adj[j].dest_id),GraphViewer::Edge::EdgeType::UNDIRECTED);

            ed.setColor(GraphViewer::BLACK);

            //ed.setThickness(0.0001);
            k++;
        }

        it++;
    }

    /*for(it_l=city_map.carrinhas[0].route.begin();it_l!=city_map.carrinhas[0].route.end();it_l++)
    {
        gv.getNode((*it_l)->info.id).setColor(GraphViewer::RED);
    }
    for(it_l=city_map.carrinhas[1].route.begin();it_l!=city_map.carrinhas[1].route.end();it_l++)
    {
        gv.getNode((*it_l)->info.id).setColor(GraphViewer::GREEN);

    }
    for(it_l=city_map.carrinhas[2].route.begin();it_l!=city_map.carrinhas[2].route.end();it_l++)
    {
        gv.getNode((*it_l)->info.id).setColor(GraphViewer::YELLOW);
    }*/
    printf("Distância percorrida pela carrinha 1: %f km\n",city_map.carrinhas[0].dist);
    printf("Distância percorrida pela carrinha 2: %f km\n",city_map.carrinhas[1].dist);
    printf("Distância percorrida pela carrinha 3: %f km\n",city_map.carrinhas[2].dist);
    list<Vertex*>::iterator it_l;

    /*for(auto i:city_map.encomendas)
    {
        gv.getNode(i).setColor(GraphViewer::CYAN);
    }*/
/*    for (Vertex *a = city_map.vertexes.at(city_map.loja); a!=city_map.vertexes.at(city_map.garagem);a=a->path)
    {
        gv.getNode(a->info.id).setColor(GraphViewer::RED);
    }
    gv.getNode(city_map.garagem).setColor(GraphViewer::GREEN);
    gv.getNode(city_map.loja).setColor(GraphViewer::YELLOW);*/

    gv.createWindow(1600, 900);

    int i = 1;

    while(gv.isWindowOpen()) {
        sleep(2);

        gv.lock();

        for(it_l=city_map.carrinhas[2].route.begin();it_l!=city_map.carrinhas[2].route.end();it_l++)
        {
            gv.getNode((*it_l)->info.id).setColor(GraphViewer::BLACK);
        }

        for(it_l=city_map.carrinhas[0].route.begin();it_l!=city_map.carrinhas[0].route.end();it_l++)
        {
            if ((*it_l)->info.is_casa) {
                gv.getNode((*it_l)->info.id).setColor(GraphViewer::CYAN);
            }
            else {
                gv.getNode((*it_l)->info.id).setColor(GraphViewer::RED);
            }
        }

        gv.unlock();

        sleep(2);

        gv.lock();

        for(it_l=city_map.carrinhas[0].route.begin();it_l!=city_map.carrinhas[0].route.end();it_l++)
        {
            gv.getNode((*it_l)->info.id).setColor(GraphViewer::BLACK);
        }

        for(it_l=city_map.carrinhas[1].route.begin();it_l!=city_map.carrinhas[1].route.end();it_l++)
        {
            if ((*it_l)->info.is_casa) {
                gv.getNode((*it_l)->info.id).setColor(GraphViewer::CYAN);
            }
            else {
                gv.getNode((*it_l)->info.id).setColor(GraphViewer::GREEN);
            }
        }

        gv.unlock();

        sleep(2);

        gv.lock();

        for(it_l=city_map.carrinhas[1].route.begin();it_l!=city_map.carrinhas[1].route.end();it_l++)
        {
            gv.getNode((*it_l)->info.id).setColor(GraphViewer::BLACK);
        }

        for(it_l=city_map.carrinhas[2].route.begin();it_l!=city_map.carrinhas[2].route.end();it_l++)
        {
            if ((*it_l)->info.is_casa) {
                gv.getNode((*it_l)->info.id).setColor(GraphViewer::CYAN);
            }
            else {
                gv.getNode((*it_l)->info.id).setColor(GraphViewer::YELLOW);
            }
        }

        gv.unlock();

        /*sleep(1);
        gv.lock();


        list<Vertex*>::iterator it_l1, it_l2, it_l3;
        it_l1 = city_map.carrinhas[0].route.begin();
        it_l2 = city_map.carrinhas[1].route.begin();
        it_l3 = city_map.carrinhas[2].route.begin();

        for (int j = 0; j < i; j++) {
            if (it_l1 != city_map.carrinhas[0].route.end()) {
                if ((*it_l1)->info.is_casa) gv.getNode((*it_l1)->info.id).setColor(GraphViewer::CYAN);
                else gv.getNode((*it_l1)->info.id).setColor(GraphViewer::RED);
                it_l1++;
            }

            if (it_l2 != city_map.carrinhas[1].route.end()) {
                if ((*it_l2)->info.is_casa) gv.getNode((*it_l2)->info.id).setColor(GraphViewer::CYAN);
                else gv.getNode((*it_l2)->info.id).setColor(GraphViewer::GREEN);
                it_l2++;
            }

            if (it_l3 != city_map.carrinhas[2].route.end()) {
                if ((*it_l3)->info.is_casa) gv.getNode((*it_l3)->info.id).setColor(GraphViewer::CYAN);
                else gv.getNode((*it_l3)->info.id).setColor(GraphViewer::YELLOW);
                it_l3++;
            }
        }

        i++;

        gv.unlock();*/
    }

    gv.join();

}

