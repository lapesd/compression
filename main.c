#include <stdbool.h>
#include "list.c"

void* arith_code(struct list* data) {
	unsigned i, j;
	bool found = false;
	short value;
	struct list* table = alloc_list();
	for(i = 0; i < data->size; i++) {
		value = get(data, i);
		for(j = 0; j < table->size && !found; j++) {
			if(get(table, j) == value)
				found = true;
		}
		if(!found) {
			add(table, value);
			found = false;
		}
	}

	struct list* counts = zeroes(table->size);
	struct list* new_data = alloc_list();
	for(i = 0; i < table->size; i++) {
		j = 1;
		while(get(table, j-1) != get(data, i))
			j++;
		add(new_data, j-1);
		substitute_at(counts, j-1, get(counts, j-1)+1);
	}
	/*  find a c equivalent to
		code = 0;
		code = arithenco(new_data, counts);
	*/

}