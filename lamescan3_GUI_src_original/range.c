/* -------------------------------------------------------------------------- */

#include "range.h"

/* -------------------------------------------------------------------------- */

range_t *range_new(range_callback_t cb, void *cb_param)
{
	range_t *range;
	if((range = malloc(sizeof(range_t))))
	{
		if((range->mutex = CreateMutex(NULL, FALSE, NULL)))
		{
			range->size = 0;
			range->items = NULL;
			range->cb = cb;
			range->cb_param = cb_param;
			return range;
		}
		free(range);
	}
	return NULL;
}

/* -------------------------------------------------------------------------- */

void range_free(range_t *range)
{
	range_elem_t *temp, *p;
	if(range)
	{
		for(p = range->items; p; p = temp)
		{
			temp = p->flink;
			free(p);
		}
		CloseHandle(range->mutex);
		free(range);
	}
}

/* -------------------------------------------------------------------------- */

int range_lock(range_t *range)
{
	int guard = 0;

	if(range)
	{
		if(WaitForSingleObject(range->mutex, INFINITE) == WAIT_OBJECT_0)
			return 1;
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

int range_unlock(range_t *range)
{
	if(range)
	{
		ReleaseMutex(range->mutex);
		return 1;
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

int range_clear(range_t *range)
{
	range_elem_t *temp, *p;
	if(range)
	{
		if(range_lock(range))
		{
			for(p = range->items; p; p = temp)
			{
				temp = p->flink;
				if(range->cb)
				{
					range->cb(range->cb_param, p, RANGE_REMOVE);
				}
				free(p);
			}
			range->size = 0;
			range->items = NULL;
			range_unlock(range);
			return 1;
		}
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

int range_merge(range_t *dest, range_t *source)
{
	int status = 0;
	range_elem_t *p, *temp;
	if( (dest) && (source) && (dest != source) )
	{
		if(range_lock(source))
		{
			status = 1;
			for(p = source->items; p; p = temp)
			{
				temp = p->flink;
				if(source->cb)
				{
					source->cb(source->cb_param, p, RANGE_REMOVE);
				}
				if(!range_add(dest, p->begin, p->end))
					status = 0;
				free(p);
			}
			source->size = 0;
			source->items = NULL;
			range_unlock(source);
		}
	}
	return status;
}

/* -------------------------------------------------------------------------- */

int range_add_single(range_t *range, unsigned int value)
{
	range_elem_t **p, *temp;
	if( (range) && (value) )
	{
		if(range_lock(range))
		{
			for(p = &(range->items); *p; p = &((*p)->flink))
			{
				if((*p)->end >= value - 1)
					break;
			}
			if(!*p)
			{
				if((temp = malloc(sizeof(range_elem_t))))
				{
					temp->begin = value;
					temp->end = value;
					temp->flink = NULL;
					*p = temp;
					if(range->cb)
					{
						range->cb(range->cb_param, temp, RANGE_ADD);
					}
					range->size++;
					range_unlock(range);
					return 1;
				}
			}
			else
			{
				if( ((*p)->begin <= value) && ((*p)->end >= value) )
				{
					range_unlock(range);
					return 1;
				}
				else if((*p)->begin - 1 == value)
				{
					(*p)->begin--;
					if(range->cb)
					{
						range->cb(range->cb_param, *p, RANGE_SET);
					}
					range->size++;
					range_unlock(range);
					return 1;
				}
				else if( (*p)->end == value - 1 )
				{
					temp = (*p)->flink;
					if( (temp) && (temp->begin - 1 == value) )
					{
						(*p)->end = temp->end;
						if(range->cb)
						{
							range->cb(range->cb_param, *p, RANGE_SET);
							range->cb(range->cb_param, temp, RANGE_REMOVE);
						}
						(*p)->flink = temp->flink;
						free(temp);
					}
					else
					{
						(*p)->end++;
						if(range->cb)
						{
							range->cb(range->cb_param, *p, RANGE_SET);
						}
					}
					range->size++;
					range_unlock(range);
					return 1;
				}
				else
				{
					if((temp = malloc(sizeof(range_elem_t))))
					{
						temp->begin = value;
						temp->end = value;
						temp->flink = *p;
						*p = temp;
						if(range->cb)
						{
							range->cb(range->cb_param, temp, RANGE_ADD);
						}
						range->size++;
						range_unlock(range);
						return 1;
					}
				}
			}
			range_unlock(range);
		}
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

int range_remove_single(range_t *range, unsigned int value)
{
	range_elem_t **p, *temp;
	if( (range) && (value) )
	{
		if(range_lock(range))
		{
			for(p = &(range->items); *p; p = &((*p)->flink))
			{
				if( ((*p)->begin <= value) && ((*p)->end >= value) )
					break;
			}
			if(*p)
			{
				if( (*p)->begin == (*p)->end )
				{
					temp = *p;
					*p = temp->flink;
					if(range->cb)
					{
						range->cb(range->cb_param, temp, RANGE_REMOVE);
					}
					free(temp);
					range->size--;
					range_unlock(range);
					return 1;
				}
				else if( (value == (*p)->begin) || (value == (*p)->end) )
				{
					if(value == (*p)->begin)
						(*p)->begin++;
					if(value == (*p)->end)
						(*p)->end--;
					if(range->cb)
					{
						range->cb(range->cb_param, *p, RANGE_SET);
					}
					range->size--;
					range_unlock(range);
					return 1;
				}
				else
				{
					if((temp = malloc(sizeof(range_elem_t))))
					{
						temp->begin = (*p)->begin;
						temp->end = value-1;
						temp->flink = *p;
						(*p)->begin = value+1;
						if(range->cb)
						{
							range->cb(range->cb_param, temp, RANGE_ADD);
							range->cb(range->cb_param, *p, RANGE_SET);
						}
						*p = temp;
						range->size--;
						range_unlock(range);
						return 1;
					}
				}
			}
			range_unlock(range);
		}
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

unsigned int range_check_single(range_t *range, unsigned int value)
{
	range_elem_t *p;
	if(range)
	{
		if(range_lock(range))
		{
			for(p = range->items; p; p = p->flink)
			{
				if((p->begin <= value) && (p->end >= value))
				{
					range_unlock(range);
					return 1;
				}
			}
			range_unlock(range);
		}
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

int range_add(range_t *range, unsigned int begin, unsigned int end)
{
	unsigned int delta;
	range_elem_t *temp, *temp2, **p;

	if(begin == end)
	{
		return range_add_single(range, begin);
	}

	if( (range) && (begin) && (begin <= end) )
	{
		if(range_lock(range))
		{
			temp = NULL;
			p = &(range->items);
			while(*p)
			{
				//новый элемент ещё не добавлен
				if(!temp)
				{
					//новый элемент не выше текущего
					if(begin - 1 <= (*p)->end)
					{
						//новый элемент пересекает текущий
						if(end >= (*p)->begin - 1)
						{
							//расширяем текущий элемент
							temp = *p;
							delta = 0;
							if(begin < temp->begin)
							{
								delta += temp->begin - begin;
								temp->begin = begin;
							}
							if(end > temp->end)
							{
								delta += end - temp->end;
								temp->end = end;
							}
							if(delta)
							{
								if(range->cb)
								{
									range->cb(range->cb_param, temp, RANGE_SET);
								}
								range->size += delta;
							}
						}

						//новый элемент целиком ниже текущего
						else
						{
							//создаём новый элемент
							if(!(temp = malloc(sizeof(range_elem_t))))
							{
								range_unlock(range);
								return 0;
							}
							temp->begin = begin;
							temp->end = end;
							temp->flink = *p;
							*p = temp;
							if(range->cb)
							{
								range->cb(range->cb_param, temp, RANGE_ADD);
							}
							range->size += end - begin + 1;
						}
					}
					p = &((*p)->flink);
				}

				//новый элемент уже добавлен - смотрим следующие элементы
				else
				{
					//новый/расширеный элемент пересекает текущий
					if( (*p)->begin - 1 <= temp->end)
					{
						delta = 0;
						//поглощаем текущий элемент
						if((*p)->end > temp->end)
						{
							delta += (*p)->end - temp->end;
							temp->end = (*p)->end;
							if(range->cb)
							{
								range->cb(range->cb_param, temp, RANGE_SET);
							}
						}
						//удаляем текущий элемент
						temp2 = *p;
						if(range->cb)
						{
							range->cb(range->cb_param, temp2, RANGE_REMOVE);
						}
						delta -= (*p)->end - (*p)->begin + 1;
						*p = temp2->flink;
						free(temp2);
						range->size += delta;
					}

					//новый/расширяемый элемент ниже текущего
					else
					{
						break;
					}
				}
			}

			//элемент нужно добавить в конец
			if(!temp)
			{
				if(!(*p = malloc(sizeof(range_elem_t))))
				{
					range_unlock(range);
					return 0;
				}
				(*p)->begin = begin;
				(*p)->end = end;
				(*p)->flink = NULL;
				if(range->cb)
				{
					range->cb(range->cb_param, *p, RANGE_ADD);
				}
				range->size += end - begin + 1;
			}
			
			range_unlock(range);
			return 1;
		}
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

int range_remove(range_t *range, unsigned int begin, unsigned int end)
{
	unsigned int delta;
	range_elem_t *temp, **p;

	if(begin == end)
	{
		return range_remove_single(range, begin);
	}

	if( (range) && (begin) && (begin <= end) )
	{
		if(range_lock(range))
		{
			p = &(range->items);
			while(*p)
			{
				//блок выше
				if((*p)->begin > end)
				{
					//завершаем
					break;
				}

				//блок ниже
				else if( (*p)->end < begin )
				{
					//идём к след. блоку
					p = &((*p)->flink);
				}
				
				//блок перекрывается
				else
				{
					//блок полностью перекрывается
					if( ((*p)->begin >= begin) && ((*p)->end <= end) )
					{
						//удаляем блок
						temp = *p;
						if(range->cb)
						{
							range->cb(range->cb_param, temp, RANGE_REMOVE);
						}
						range->size -= temp->end - temp->begin + 1;
						*p = temp->flink;
						free(temp);
					}

					//перекрывается слева
					else if( begin <= (*p)->begin )
					{
						delta = end - (*p)->begin + 1;
						(*p)->begin = end + 1;
						if(range->cb)
						{
							range->cb(range->cb_param, *p, RANGE_SET);
						}
						range->size -= delta;
						p = &((*p)->flink);
					}

					//перекрывается справа
					else if( end >= (*p)->end )
					{
						delta = (*p)->end - begin + 1;
						(*p)->end = begin - 1;
						if(range->cb)
						{
							range->cb(range->cb_param, *p, RANGE_SET);
						}
						range->size -= delta;
						p = &((*p)->flink);
					}

					//перекрывается в середине
					else
					{
						if(!(temp = malloc(sizeof(range_elem_t))))
						{
							range_unlock(range);
							return 0;
						}
						temp->begin = (*p)->begin;
						temp->end = begin - 1;
						temp->flink = *p;
						(*p)->begin = end + 1;
						if(range->cb)
						{
							range->cb(range->cb_param, temp, RANGE_ADD);
							range->cb(range->cb_param, *p, RANGE_SET);
						}
						*p = temp;
						range->size -= end - begin + 1;
					}
				}
			}
			
			range_unlock(range);
			return 1;
		}
	}
	return 0;
}

/* -------------------------------------------------------------------------- */

unsigned int range_check(range_t *range, unsigned int begin, unsigned int end)
{
	unsigned int count = 0;
	unsigned int begin1, end1;
	range_elem_t *p;

	if(begin == end)
	{
		return range_check_single(range, begin);
	}

	if( (range) && (begin) && (begin <= end) )
	{
		if(range_lock(range))
		{
			for(p = range->items; p; p = p->flink)
			{
				begin1 = max(p->begin, begin);
				end1 = min(p->end, end);
				if(begin1 <= end1)
				{
					count += end1 - begin1 + 1;
				}
			}
			range_unlock(range);
		}
	}
	return count;
}

/* -------------------------------------------------------------------------- */

unsigned int range_get_nolock(range_t *range, unsigned int index, int remove)
{
	unsigned int size, value;
	range_elem_t **p, *temp;
	if( (range) && (index < range->size) )
	{
		for(p = &(range->items); *p; p = &((*p)->flink))
		{
			size = (*p)->end - (*p)->begin + 1;
			if(index < size)
			{
				value = (*p)->begin + index;
				if(remove)
				{
					//элемент полностью удаляется
					if((*p)->begin == (*p)->end)
					{
						if(range->cb)
						{
							range->cb(range->cb_param, *p, RANGE_REMOVE);
						}
						temp = *p;
						*p = temp->flink;
						free(temp);
					}

					//удаляем слева
					else if(index == 0)
					{
						(*p)->begin++;
						if(range->cb)
						{
							range->cb(range->cb_param, *p, RANGE_SET);
						}
					}

					//удаляем справа
					else if(index == size - 1)
					{
						(*p)->end--;
						if(range->cb)
						{
							range->cb(range->cb_param, *p, RANGE_SET);
						}
					}

					//удаляем из середины
					else 
					{
						if(!(temp = malloc(sizeof(range_elem_t))))
						{
							range_unlock(range);
							return 0;
						}
						temp->begin = (*p)->begin;
						temp->end = value - 1;
						temp->flink = *p;
						(*p)->begin = value + 1;
						if(range->cb)
						{
							range->cb(range->cb_param, temp, RANGE_ADD);
							range->cb(range->cb_param, *p, RANGE_SET);
						}
						*p = temp;
					}
					
					range->size--;
				}
				range_unlock(range);
				return value;
			}
			index -= size;
		}
}
	return 0;
}

/* -------------------------------------------------------------------------- */

unsigned int range_get_first(range_t *range, int remove)
{
	unsigned int result = 0;
	if(range)
	{
		if(range_lock(range))
		{
			if(range->size > 0)
			{
				result = range_get_nolock(range, 0, remove);
			}
			range_unlock(range);
		}
	}
	return result;
}

/* -------------------------------------------------------------------------- */

unsigned int range_get_random(range_t *range, int remove)
{
	int i;
	unsigned int result = 0;
	unsigned int index;
	unsigned char *p;
	if(range)
	{
		if(range_lock(range))
		{
			if(range->size > 0)
			{
				p = (void*)&index;
				for(i = 0; i < 4; ++i)
					p[i] = rand();
				index %= range->size;
				result = range_get_nolock(range, index, remove);
			}
			range_unlock(range);
		}
	}
	return result;
}

/* -------------------------------------------------------------------------- */
