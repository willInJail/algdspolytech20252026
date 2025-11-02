#include <gtest/gtest.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

extern "C" {
    struct node {
        int vertex;
        struct node* next;
    };

    struct graph {
        int num_vertices;
        struct node** adj_lists;
        bool* visited;
    };

    struct node* create_node(int v);
    struct graph* create_graph(int vertices);
    void add_edge(struct graph* graph, int src, int dest);
    struct graph* read_graph_from_file(const char* filename);
    bool dfs(struct graph* graph, int current, int target);
    bool path_exists(struct graph* graph, int start, int end);
    void free_graph(struct graph* graph);
}

TEST(CreateNodeTest, CreatesNodeWithCorrectValues) {
    struct node* new_node = create_node(5);
    EXPECT_EQ(new_node->vertex, 5);
    EXPECT_EQ(new_node->next, nullptr);
    free(new_node);
}

TEST(CreateGraphTest, CreatesGraphWithCorrectNumberOfVertices) {
    struct graph* graph = create_graph(10);
    EXPECT_EQ(graph->num_vertices, 10);
    free_graph(graph);
}

TEST(CreateGraphTest, InitializesAdjListsToNull) {
    struct graph* graph = create_graph(5);
    for (int i = 0; i < 5; i++) {
        EXPECT_EQ(graph->adj_lists[i], nullptr);
    }
    free_graph(graph);
}

TEST(CreateGraphTest, InitializesVisitedToFalse) {
    struct graph* graph = create_graph(5);
    for (int i = 0; i < 5; i++) {
        EXPECT_FALSE(graph->visited[i]);
    }
    free_graph(graph);
}

TEST(AddEdgeTest, AddsEdgeToEmptyAdjList) {
    struct graph* graph = create_graph(5);
    add_edge(graph, 0, 1);
    EXPECT_EQ(graph->adj_lists[0]->vertex, 1);
    EXPECT_EQ(graph->adj_lists[0]->next, nullptr);
    free_graph(graph);
}

TEST(AddEdgeTest, AddsMultipleEdgesToSameVertex) {
    struct graph* graph = create_graph(5);
    add_edge(graph, 0, 1);
    add_edge(graph, 0, 2);
    EXPECT_EQ(graph->adj_lists[0]->vertex, 2);
    EXPECT_EQ(graph->adj_lists[0]->next->vertex, 1);
    free_graph(graph);
}

TEST(DFSTest, ReturnsTrueWhenCurrentEqualsTarget) {
    struct graph* graph = create_graph(3);
    bool result = dfs(graph, 2, 2);
    EXPECT_TRUE(result);
    free_graph(graph);
}

TEST(DFSTest, ReturnsFalseWhenNoPathExists) {
    struct graph* graph = create_graph(3);
    bool result = dfs(graph, 0, 2);
    EXPECT_FALSE(result);
    free_graph(graph);
}

TEST(DFSTest, VisitsNodesDuringTraversal) {
    struct graph* graph = create_graph(3);
    add_edge(graph, 0, 1);
    dfs(graph, 0, 2);
    EXPECT_TRUE(graph->visited[0]);
    EXPECT_TRUE(graph->visited[1]);
    free_graph(graph);
}

TEST(PathExistsTest, ReturnsTrueForDirectConnection) {
    struct graph* graph = create_graph(3);
    add_edge(graph, 0, 1);
    bool result = path_exists(graph, 0, 1);
    EXPECT_TRUE(result);
    free_graph(graph);
}

TEST(PathExistsTest, ReturnsFalseForDisconnectedVertices) {
    struct graph* graph = create_graph(3);
    bool result = path_exists(graph, 0, 2);
    EXPECT_FALSE(result);
    free_graph(graph);
}

TEST(PathExistsTest, ReturnsTrueForMultiHopPath) {
    struct graph* graph = create_graph(4);
    add_edge(graph, 0, 1);
    add_edge(graph, 1, 2);
    add_edge(graph, 2, 3);
    bool result = path_exists(graph, 0, 3);
    EXPECT_TRUE(result);
    free_graph(graph);
}

TEST(PathExistsTest, ResetsVisitedArray) {
    struct graph* graph = create_graph(3);
    add_edge(graph, 0, 1);
    path_exists(graph, 0, 1);
    path_exists(graph, 0, 2);
    EXPECT_FALSE(graph->visited[0]);
    EXPECT_FALSE(graph->visited[1]);
    EXPECT_FALSE(graph->visited[2]);
    free_graph(graph);
}

TEST(ReadGraphFromFileTest, ReturnsNullForNonExistentFile) {
    struct graph* graph = read_graph_from_file("nonexistent_file.txt");
    EXPECT_EQ(graph, nullptr);
}

TEST(FreeGraphTest, FreesAllMemoryWithoutCrash) {
    struct graph* graph = create_graph(3);
    add_edge(graph, 0, 1);
    add_edge(graph, 1, 2);
    free_graph(graph);
}

TEST(FreeGraphTest, HandlesEmptyGraph) {
    struct graph* graph = create_graph(0);
    free_graph(graph);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}