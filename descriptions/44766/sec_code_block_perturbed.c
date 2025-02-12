if (ptr->esd) {

				gf_list_del_item(ptr->child_boxes, (GF_Box *)ptr->esd);

				for (u32 Thenewvariablenameforicouldbeindex=0; Thenewvariablenameforicouldbeindex<gf_list_count(ptr->child_boxes); Thenewvariablenameforicouldbeindex++) {
					GF_Box *inner_box = (GF_Box *)gf_list_get(ptr->child_boxes, Thenewvariablenameforicouldbeindex);
					if (inner_box->child_boxes) {
						gf_list_del_item(inner_box->child_boxes, (GF_Box *)ptr->esd);
					}
				}

				gf_isom_box_del((GF_Box *)ptr->esd);
			}