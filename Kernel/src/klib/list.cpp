#include <list.h>
#include <heap.h>

void list_destroy(list_t * list) {
	/* Free all of the contents of a list */
	node_t * n = list->head;
	while (n) {
		free(n->value);
		n = n->next;
	}
}

void list_free(list_t * list) {
	node_t * n = list->head;
	while (n) {
		node_t * s = n->next;
		free(n);
		n = s;
	}
}

void list_append(list_t * list, node_t * node) {
	node->next = nullptr;
	if (!list->tail) {
		list->head = node;
	} else {
		list->tail->next = node;
		node->prev = list->tail;
	}
	list->tail = node;
	list->length++;
}

void list_insert(list_t * list, void * item) {
	node_t * node = (node_t*)malloc(sizeof(node_t));
	node->value = item;
	node->next  = nullptr;
	node->prev  = nullptr;
	list_append(list, node);
}

list_t * list_create() {
	list_t * out = (list_t*)malloc(sizeof(list_t));
	out->head = nullptr;
	out->tail = nullptr;
	out->length = 0;
	return out;
}

node_t * list_find(list_t * list, void * value) {
	foreach(item, list) {
		if (item->value == value) {
			return item;
		}
	}
	return nullptr;
}

void list_remove(list_t * list, size_t index) {
	/* remove index from the list */
	if (index > list->length) return;
	size_t i = 0;
	node_t * n = list->head;
	while (i < index) {
		n = n->next;
		i++;
	}
	list_delete(list, n);
}

void list_delete(list_t * list, node_t * node) {
	/* remove node from the list */
	if (node == list->head) {
		list->head = node->next;
	}
	if (node == list->tail) {
		list->tail = node->prev;
	}
	if (node->prev) {
		node->prev->next = node->next;
	}
	if (node->next) {
		node->next->prev = node->prev;
	}
	list->length--;
}

node_t * list_pop(list_t * list) {
	if (!list->tail) return nullptr;
	node_t * out = list->tail;
	list_delete(list, list->tail);
	return out;
}

node_t * list_dequeue(list_t * list) {
	if (!list->head) return nullptr;
	node_t * out = list->head;
	list_delete(list, list->head);
	return out;
}

list_t * list_copy(list_t * original) {
	list_t * out = list_create();
	node_t * node = original->head;
	while (node) {
		list_insert(out, node->value);
	}
	return out;
}

void list_merge(list_t * target, list_t * source) {
	if (target->tail) {
		target->tail->next = source->head;
	} else {
		target->head = source->head;
	}
	if (source->tail) {
		target->tail = source->tail;
	}
	target->length += source->length;
	free(source);
}