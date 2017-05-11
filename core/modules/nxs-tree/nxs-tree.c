// clang-format off

/* Module includes */

#include <nxs-core/nxs-core.h>

/* Module definitions */



/* Module typedefs */



/* Module declarations */



/* Module internal (static) functions prototypes */

// clang-format on

// clang-format off

/* Module initializations */



/* Module global functions */

// clang-format on

/*
 * TODO: Отсутсвуют функции удаления элементов, разрушения дерева и освобождения памяти
 */

nxs_tree_t *nxs_tree_malloc(u_int size)
{
	nxs_tree_t *tree = NULL;

	tree = (nxs_tree_t *)nxs_malloc(tree, sizeof(nxs_tree_t));

	nxs_tree_init(tree, size);

	return tree;
}

nxs_tree_t *nxs_tree_destroy(nxs_tree_t *tree)
{

	if(tree == NULL) {

		return NULL;
	}

	//	nxs_tree_free(tree);

	return nxs_free(tree);
}

void *nxs_tree_init(nxs_tree_t *tree, u_int size)
{

	tree->root = (nxs_tree_el_t *)nxs_calloc(NULL, sizeof(nxs_tree_el_t));

	tree->root->childs       = NULL;
	tree->root->childs_count = 0;
	tree->root->parent       = NULL;
	tree->root->data         = nxs_calloc(tree->root->data, size);

	tree->c_el = tree->c_root = tree->root;

	tree->size  = size;
	tree->count = 1;

	return tree->c_el->data;
}

/*
void nxs_tree_free(nxs_tree_t *tree)
{


}
*/

/*
 * Добавление нового элемента в текущий корень
 */
void *nxs_tree_add(nxs_tree_t *tree)
{
	nxs_tree_el_t *p = NULL;

	if(tree == NULL) {

		return NULL;
	}

	p = (nxs_tree_el_t *)nxs_calloc(p, sizeof(nxs_tree_el_t));

	p->childs       = NULL;
	p->childs_count = 0;
	p->parent       = tree->c_root;
	p->data         = nxs_malloc(p->data, tree->size);

	tree->c_root->childs_count++;

	tree->c_root->childs = (nxs_tree_el_t **)nxs_realloc(tree->c_root->childs, sizeof(nxs_tree_el_t *) * tree->c_root->childs_count);

	tree->c_root->childs[tree->c_root->childs_count - 1] = p;

	tree->c_el = p;

	tree->count++;

	return p->data;
}

/*
 * Удаление i-го (дочернего) элемента в текущем корне
 *
 * mode - это способ удаления элемента:
 * * NXS_TREE_DEL_CHLD_ORDER	- сохранить порядок следования элементов (элементы-потомки будут перемещены на один влево (от удаляемого))
 * * NXS_TREE_DEL_CHLD_TAIL		- поместить на место удаляемого элемента элемент с конца (более быстрый способ, если не важен порядок
 * элементов)
 */
int nxs_tree_del_child(nxs_tree_t *tree, int index, int mode)
{
	int            i;
	nxs_tree_el_t *t, *p;

	if(tree == NULL) {

		return NXS_TREE_DEL_CHLD_ERR_PTR;
	}

	p = tree->c_root;

	if(p == NULL) {

		return NXS_TREE_DEL_CHLD_ERR_PTR;
	}

	if(index < 0 || index >= p->childs_count) {

		return NXS_TREE_DEL_CHLD_ERR_INDEX;
	}

	t = p->childs[index];

	if(t->childs_count > 0) {

		/*
		 * Удаляемый элемент является корнем поддерева - его удаление может привести к утечке памяти
		 */

		return NXS_TREE_DEL_CHLD_ERR_CHLD;
	}

	t->data = nxs_free(t->data);
	t       = nxs_free(t);

	p->childs_count--;

	if(p->childs_count == 0) {

		p->childs = nxs_free(p->childs);
	}
	else {

		if(mode == NXS_TREE_DEL_CHLD_ORDER) {

			for(i = index; i < p->childs_count; i++) {

				p->childs[i] = p->childs[i + 1];
			}
		}
		else {

			p->childs[index] = p->childs[p->childs_count];
		}

		p->childs = (nxs_tree_el_t **)nxs_realloc(p->childs, sizeof(nxs_tree_el_t *) * tree->c_root->childs_count);
	}

	tree->count--;

	return NXS_TREE_DEL_CHLD_OK;
}

/*
 * Изменение текущего корня (на текущий элемент)
 */
void nxs_tree_chroot(nxs_tree_t *tree)
{

	if(tree->c_el != NULL) {

		tree->c_root = tree->c_el;
	}
}

/*
 * Изменение текущего корня (на уровень выше)
 */
void nxs_tree_chroot_up(nxs_tree_t *tree)
{

	if(tree->c_root->parent != NULL) {

		tree->c_root = tree->c_root->parent;
	}
}

/*
 * Изменение указателя на текущий элемент (указатель смещается на текущий корень)
 */
void *nxs_tree_ptr_c_el_init(nxs_tree_t *tree)
{

	tree->c_el = tree->c_root;

	return tree->c_el->data;
}

/*
 * Изменение указателя на текущий корень (указатель смещается на основной корень)
 */
void *nxs_tree_ptr_c_root_init(nxs_tree_t *tree)
{

	tree->c_root = tree->root;

	return tree->c_root->data;
}

/*
 * Задать указатель на текущий элемент, равный указателю с номером "index" в массиве потомков текущего корня
 */
void *nxs_tree_ptr_c_el_set(nxs_tree_t *tree, int index)
{

	if(tree->c_root == NULL) {

		return NULL;
	}

	if(index >= 0 && index < tree->c_root->childs_count) {

		tree->c_el = tree->c_root->childs[index];

		return tree->c_el->data;
	}

	return NULL;
}

/*
 * Получить данные родительского элемента для текущего корня
 */
void *nxs_tree_get_c_root_parent(nxs_tree_t *tree)
{

	if(tree == NULL) {

		return NULL;
	}

	if(tree->c_root == NULL) {

		return NULL;
	}

	if(tree->c_root->parent == NULL) {

		return NULL;
	}

	return tree->c_root->parent->data;
}

/*
 * Получить данные текущего корня
 */
void *nxs_tree_get_c_root(nxs_tree_t *tree)
{

	if(tree->c_root == NULL) {

		return NULL;
	}

	return tree->c_root->data;
}

/*
 * Получить данные текущего элемента
 */
void *nxs_tree_get_c_el(nxs_tree_t *tree)
{

	if(tree->c_el == NULL) {

		return NULL;
	}

	return tree->c_el->data;
}

int nxs_tree_get_root_cc(nxs_tree_t *tree)
{
	if(tree == NULL) {

		return -1;
	}

	if(tree->root == NULL) {

		return -1;
	}

	return tree->root->childs_count;
}

int nxs_tree_get_c_root_cc(nxs_tree_t *tree)
{
	if(tree == NULL) {

		return -1;
	}

	if(tree->c_root == NULL) {

		return -1;
	}

	return tree->c_root->childs_count;
}

int nxs_tree_get_c_el_cc(nxs_tree_t *tree)
{
	if(tree == NULL) {

		return -1;
	}

	if(tree->c_el == NULL) {

		return -1;
	}

	return tree->c_el->childs_count;
}

/* Module internal (static) functions */
