#include "enlightenment.h"

listhead *ListInit() {
	listhead *lh;
	lh=(listhead *)malloc(sizeof(listhead));
	lh->num=0;
	lh->first=NULL;
	lh->last=NULL;
	return lh;
}

void ToFront(EWin *ewin) {
	struct list *ptr;
	struct list *pptr;

	ptr=global_l->first;
	if (ptr) ptr->win->top=0;
	ewin->top=1;
	pptr=NULL;
	while(ptr) {
		if (ptr->win==ewin) {
			if (pptr) {
				pptr->next=ptr->next;
				ptr->next=global_l->first;
				global_l->first=ptr;
				if (global_l->last==ptr) 
					global_l->last=pptr;
			}
		}
		pptr=ptr;
		ptr=ptr->next;
	}
}

void ToBack(EWin *ewin) {
	struct list *ptr;
	struct list *pptr;

	ptr=global_l->first;
	if (ptr) 
		ptr->win->top=1;
	ewin->top=0;
	pptr=NULL;
	while(ptr) {
		if (ptr->win==ewin) {
			if (pptr) {
				if (ptr!=global_l->last) {
					pptr->next=ptr->next;
					ptr->next=NULL;
					global_l->last->next=ptr;
					global_l->last=ptr;
				}
			} else {
				global_l->last->next=ptr;
				global_l->last=ptr;
				global_l->first=ptr->next;
				ptr->next=NULL;
			}
		}
		pptr=ptr;
		ptr=ptr->next;
	}
}

void ListKill(listhead *l) {
	struct list *node;

	node=l->first;
	while(node) {
		KillEWin(node->win);
		node=node->next;
		free(node);
	}
	free(l);
}

void ListAdd(listhead *l, EWin *data) {
	struct list *node;


	data->top=1;
	if (l->first==NULL) {
		l->first=(struct list*)malloc(sizeof(struct list));
		l->last=l->first;
		if(l->first==NULL) {
			fprintf(stderr,"Enlightenment: FATAL!!!!! PANIC!!!!!\nCannont malloc memory for vital list element\n");
			EExit(1);
		}
		l->first->next=NULL;
		l->first->win=data;
		l->num++;
	} else {
		l->first->win->top=0;
		node=(struct list*)malloc(sizeof(struct list));
		if(node==NULL) {
			fprintf(stderr,"Enlightenment: FATAL!!!!! PANIC!!!!!\nCannont malloc memory for vital list element\n");
			EExit(1);
		}
		node->next=l->first;
		l->first=node;
		node->win=data;
		l->num++;
	}
}

void ListDelWinID(listhead *l, Window w) {
	struct list *node;
	struct list *pnode;

	pnode=NULL;
	node=l->first;
	while(node) {
		if (node->win->frame_win==w) {
			if (l->num<=1) {
				l->first=NULL;
				l->last=NULL;
				l->num=0;
				KillEWin(node->win);
				free(node);
				return;
			} else if (pnode==NULL) {
				l->first=node->next;
				KillEWin(node->win);
				free(node);
				l->num--;
				return;
			} else {
				pnode->next=node->next;
				if (node->next==NULL) 
					l->last=pnode;
				KillEWin(node->win);
				free(node);
				l->num--;
				return;
			}
		}
		pnode=node;
		node=node->next;
	}
}


EWin *ListGetWinID(listhead *l, Window w) {
	struct list *node;

	node=l->first;
	while(node) {
		if (node->win->frame_win==w)
			return node->win;
		node=node->next;
	}
	return NULL;
}

EWin *ListGetClientWinID(listhead *l, Window w) {
	struct list *node;

	node=l->first;
	while(node) {
		if (node->win->client_win==w)
			return node->win;
		node=node->next;
	}
	return NULL;
}

EWin *ListGetSubWinID(listhead *l, Window w) {
	struct list *node;
	int i;

	node=l->first;
	while(node) {
		for (i=0;i<node->win->num_subwins;i++)
			if (node->win->subwins[i]==w)
				return node->win;
		node=node->next;
	}
	return NULL;
}
