#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cstring>

using namespace std;

const int SIZE = 100;
const int MAX = 0x7fffffff;

struct graph
{
    int vertex[SIZE];
    int mp[SIZE][SIZE];
    int type;
    int n;
    int e;
    graph()
    {
        for(int i = 0; i < SIZE; i++)
        {
            vertex[i] = MAX;
            for(int j = 0; j < SIZE; j++)
            {
                mp[i][j] = MAX;
            }
        }
    }
}mat;

struct node
{
    int adjvex;
    int cost;
    struct node *next;
    node() {next = NULL;}
};

struct vnode
{
    int vertex;
    node *firstedge;
    vnode() {firstedge = NULL;}
};

struct adjlist
{
    vnode vexlist[SIZE];
    int n;
    int e;
    int type;   //0 :AL     1 :PAL      2 :IAL
}adjgraph;

struct arcbox
{
    int tailvex;
    int headvex;
    struct arcbox *hlink;
    struct arcbox *tlink;
    int info;
    arcbox() {hlink = NULL; tlink = NULL;}
};

struct vexnode
{
    int data;
    arcbox *firstin;
    arcbox *firstout;
    vexnode() {firstin = NULL; firstout = NULL;}
};

struct OLList
{
    vexnode xlist[SIZE];
    int vexnum;
    int arcnum;
}olgraph;

enum visstat {unvis, vis};

struct ebox
{
    visstat mark;
    int ivex;
    int jvex;
    struct ebox *ilink;
    struct ebox *jlink;
    int info;
    ebox() {ilink = NULL; jlink = NULL; mark = unvis;}
};

struct vexbox
{
    int data;
    ebox *firstedge;
    vexbox() {firstedge = NULL;}
};

struct AML
{
    vexbox adjmulist[SIZE];
    int vexnum;
    int edgenum;
}amlgraph;

void readFile(int flag);
void mat2PAL(void);
void printPAL(void);
void mat2OLG(void);
void printOLG(void);
void mat2AML(void);
void printAML(void);
void dfs(node *edge);
void PALDFS(void);
void matBFS(void);
void prim(void);
void dijkstra(int src);
void OLG_DFS(int v);
void OLG_Rev_DFS(int v);
void korasaju(void);

void readFile(int flag)
{
    FILE *fp;
    char str[20];
    sprintf(str, "src%02d.txt", flag);
    if((fp = fopen(str, "r")) == NULL)
    {
        printf("Sorry, can't open this file.");
        exit(1);
    }
    fscanf(fp, "%d%d", &mat.type, &mat.n);
    int cnt = 0;
    while(!feof(fp))
    {
        int i, j;
        fscanf(fp, "%d%d", &i, &j);
        fscanf(fp, "%d", mat.mp[i] + j);
        if(mat.type == 0)
        {
            mat.mp[j][i] = mat.mp[i][j];
        }
        cnt++;
    }
    mat.e = cnt;
}

void mat2PAL(void)
{
    adjgraph.type = 1;
    adjgraph.n = mat.n;
    int cnt = 0;
    for(int i = 0; i < mat.n; i++)
    {
        bool flag = false;
        node *q = NULL;
        adjgraph.vexlist[i].vertex = mat.vertex[i];
        for(int j = 0; j < mat.n; j++)
        {
            if(i == j)
            {
                continue;
            }
            if(mat.mp[i][j] != MAX)
            {
                node *p = new node;
                p->cost = mat.mp[i][j];
                p->adjvex = j;
                p->next = NULL;
                if(!flag)
                {
                    adjgraph.vexlist[i].firstedge = p;
                    q = p;
                    flag = true;
                }
                else
                {
                    q->next = p;
                    q = q->next;
                }
                cnt++;
            }
        }
    }
    adjgraph.e = cnt;
}

void printPAL(void)
{
    for(int i = 0; i < adjgraph.n; i++)
    {
        printf("vertex[%d]", i);
        node *p = adjgraph.vexlist[i].firstedge;
        while(p != NULL)
        {
            printf(" -> %d", p->adjvex);
            p = p->next;
        }
        printf("\n");
    }
}

void mat2OLG(void)
{
    olgraph.vexnum = mat.n;
    olgraph.arcnum = mat.e;
    for(int i = 0; i < olgraph.vexnum; i++)
    {
        olgraph.xlist[i].data = mat.vertex[i];
    }
    for(int i = 0; i < mat.n; i++)
    {
        for(int j = 0; j < mat.n; j++)
        {
            if(mat.mp[i][j] != MAX)
            {
                arcbox *p = new arcbox;
                p->tailvex = i;
                p->headvex = j;
                p->hlink = olgraph.xlist[j].firstin;
                p->tlink = olgraph.xlist[i].firstout;
                p->info = mat.mp[i][j];
                olgraph.xlist[j].firstin = p;
                olgraph.xlist[i].firstout = p;
                //printf("(%d, %d, %d)\n", p->tailvex, p->headvex, p->info);
            }
        }
    }
}

void printOLG(void)
{
    for(int i = 0; i < olgraph.vexnum; i++)
    {
        arcbox *p = olgraph.xlist[i].firstin;
        while(p != NULL)
        {
            printf("(%d -> %d: %d)\n", p->tailvex, p->headvex, p->info);
            p = p->hlink;
        }
    }
    printf("\n");
    for(int i = 0; i < olgraph.vexnum; i++)
    {
        arcbox *p = olgraph.xlist[i].firstout;
        while(p != NULL)
        {
            printf("(%d <- %d: %d)\n", p->headvex, p->tailvex, p->info);
            p = p->tlink;
        }
    }
}

void mat2AML(void)
{
    amlgraph.vexnum = mat.n;
    amlgraph.edgenum = mat.e;
    for(int i = 0; i < amlgraph.vexnum; i++)
    {
        amlgraph.adjmulist[i].data = mat.vertex[i];
    }
    for(int i = 0; i < mat.n; i++)
    {
        for(int j = i + 1; j < mat.n; j++)
        {
            if(mat.mp[i][j] != MAX)
            {
                ebox *p = new ebox;
                p->ivex = i;
                p->jvex = j;
                p->ilink = amlgraph.adjmulist[i].firstedge;
                p->jlink = amlgraph.adjmulist[j].firstedge;
                p->info = mat.mp[i][j];
                amlgraph.adjmulist[i].firstedge = p;
                amlgraph.adjmulist[j].firstedge = p;
                //printf("(%d, %d, %d)\n", p->ivex, p->jvex, p->info);
            }
        }
    }
}

void printAML(void)
{
    for(int i = 0; i < amlgraph.vexnum; i++)
    {
        ebox *p = amlgraph.adjmulist[i].firstedge;
        while(p != NULL)
        {
            if(p->mark == unvis)
            {
                printf("(%d -- %d: %d)\n", p->ivex, p->jvex, p->info);
                p->mark = vis;
            }
            p = p->ilink;
        }
    }
}

bool visit[SIZE];

void dfs(node *edge)
{
    while(edge != NULL)
    {
        int temp = edge->adjvex;
        if(!visit[temp])
        {
            printf(" -> v[%d]", temp);
            visit[temp] = true;
            dfs(adjgraph.vexlist[temp].firstedge);
        }
        edge = edge->next;
    }
}

void PALDFS(void)
{
    if(adjgraph.type != 1)
    {
        printf("此表不是正邻接表！\n");
        return;
    }
    for(int i = 0; i < adjgraph.n; i++)
    {
        visit[i] = false;
    }
    for(int i = 0; i < adjgraph.n; i++)
    {
        if(!visit[i])
        {
            visit[i] = true;
            printf("v[%d]", i);
            dfs(adjgraph.vexlist[i].firstedge);
            printf("\n");
        }
    }
}

void matBFS(void)
{
    int q[SIZE + 20];
    int _front = 0;
    int _rear = 0;
    for(int i = 0; i < mat.n; i++)
    {
        visit[i] = false;
    }
    for(int i = 0; i < mat.n; i++)
    {
        if(!visit[i])
        {
            q[_rear++] = i;
            visit[i] = true;
            while(_front != _rear)
            {
                int temp = q[_front++];
                printf("v[%d] ", temp);
                for(int j = 0; j < mat.n; j++)
                {
                    if(!visit[j] && mat.mp[temp][j] != MAX)
                    {
                        visit[j] = true;
                        q[_rear++] = j;
                    }
                }
            }
        }
    }
}

void prim(void)
{
    int lowcost[SIZE];
    int mst[SIZE];
    int sum = 0;
    for(int i = 1; i < mat.n; i++)
    {
        lowcost[i] = mat.mp[0][i];
        mst[i] = 0;
    }
    mst[0] = -1;
    for(int i = 1; i < mat.n; i++)
    {
        int _min = MAX;
        int minid = 0;
        for(int j = 1; j < mat.n; j++)
        {
            if(lowcost[j] < _min && lowcost[j] != 0)
            {
                _min = lowcost[j];
                minid = j;
            }
        }
        printf("v[%d] -> v[%d]: %d\n", mst[minid], minid, _min);
        sum += _min;
        lowcost[minid] = 0;
        for(int j = 1; j < mat.n; j++)
        {
            if(mat.mp[minid][j] < lowcost[j])
            {
                lowcost[j] = mat.mp[minid][j];
                mst[j] = minid;
            }
        }
    }
    printf("MST总长度：%d\n", sum);
}

void dijkstra(int src)
{
    bool s[SIZE];
    int dist[SIZE];
    int prev[SIZE];
    *prev = 0;  /*just make the warning disappear*/
    for(int i = 0; i < mat.n; i++)
    {
        dist[i] = mat.mp[src][i];
        s[i] = false;
        prev[i] = ((dist[i] == MAX) ? -1 : src);
    }
    dist[src] = 0;
    s[src] = true;
    for(int i = 1; i < mat.n; i++)
    {
        int _min = MAX;
        int temp = src;
        for(int j = 0; j < mat.n; j++)
        {
            if(!s[j] && dist[j] < _min)
            {
                temp = j;
                _min = dist[j];
            }
        }
        s[temp] = true;
        for(int j = 0; j < mat.n; j++)
        {
            if(!s[j] && mat.mp[temp][j] < MAX)
            {
                int t = dist[temp] + mat.mp[temp][j];
                if(t < dist[j])
                {
                    dist[j] = t;
                    prev[j] = temp;
                }
            }
        }
    }
    for(int i = 0; i < mat.n; i++)
    {
        if(i != src)
        {
            printf("v[%d] -> v[%d]: %d\n", src, i, dist[i]);
        }
    }
}

int in_order[SIZE];
int cnt;

void OLG_DFS(int v)
{
    arcbox *p = olgraph.xlist[v].firstout;
    visit[v] = true;
    while(p != NULL)
    {
        if(!visit[p->headvex])
        {
            OLG_DFS(p->headvex);
        }
        p = p->tlink;
    }
    in_order[cnt++] = v;
}

void OLG_Rev_DFS(int v)
{
    arcbox *p = olgraph.xlist[v].firstin;
    visit[v] = true;
    printf("%d ", v);
    while(p != NULL)
    {
        if(!visit[p->tailvex])
        {
            OLG_Rev_DFS(p->tailvex);
        }
        p = p->hlink;
    }
}

void korasaju(void)
{
    int cnt = 0;
    for(int i = 0; i < olgraph.vexnum; i++)
    {
        visit[i] = false;
    }
    for(int i = 0; i < olgraph.vexnum; i++)
    {
        if (!visit[i])
        {
            OLG_DFS(i);
        }
    }
    for(int i = 0; i < olgraph.vexnum; i++)
    {
        visit[i] = false;
    }
    for(int i = olgraph.vexnum - 1; i >= 0; i--)
    {
        int j = in_order[i];
        if (!visit[j])
        {
            printf("\n第%d个连通分量顶点:", ++cnt);
            OLG_Rev_DFS(j);
        }
    }
    printf("\n");
}

void topsort(void)
{
    int indeg[SIZE];
    int s[SIZE];
    int top = -1;
    memset(indeg, 0, sizeof(indeg));
    for(int i = 0; i < adjgraph.n; i++)
    {
        node *p = adjgraph.vexlist[i].firstedge;
        while(p != NULL)
        {
            indeg[p->adjvex]++;
            p = p->next;
        }
    }
    for(int i = 0; i < adjgraph.n; i++)
    {
        if(indeg[i] == 0)
        {
            s[++top] = i;
        }
    }
    int cnt = 0;
    while(top != -1)
    {
        int t = s[top--];
        printf("%d ", t);
        cnt++;
        node *p = adjgraph.vexlist[t].firstedge;
        while(p != NULL)
        {
            int k = p->adjvex;
            indeg[k]--;
            if(indeg[k] == 0)
            {
                s[++top] = k;
            }
            p = p->next;
        }
    }
    printf("\n");
    if(cnt < adjgraph.n)
    {
        printf("图中存在回路\n");
    }
}

int main()
{
    int num;
    printf("请输入你想加载的源文件编号\n");
    printf("0：简单无向图\n");
    printf("1：简单有向图\n");
    printf("2：复杂有向图\n");
    printf("3：复杂无向图\n");
    printf("4：MST-PRIM\n");
    printf("5：强连通分量1\n");
    printf("6：强连通分量2\n");
    printf("**************************\n");
    scanf("%d", &num);
    readFile(num);
    if(mat.type == 1)
    {
        mat2PAL();
        printPAL();
        mat2OLG();
        printOLG();
        PALDFS();
        matBFS();
        cnt = 0;
        korasaju();
        topsort();
    }
    else
    {
        mat2AML();
        printAML();
        prim();
        dijkstra(0);
    }
    return 0;
}
