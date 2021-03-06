/*
 *  Copyright (c) 2010 The WebM project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <stdio.h>

#include "vp10/common/blockd.h"
#include "vp10/common/onyxc_int.h"

static void log_frame_info(VP10_COMMON *cm, const char *str, FILE *f) {
  fprintf(f, "%s", str);
  fprintf(f, "(Frame %d, Show:%d, Q:%d): \n", cm->current_video_frame,
          cm->show_frame, cm->base_qindex);
}
/* This function dereferences a pointer to the mbmi structure
 * and uses the passed in member offset to print out the value of an integer
 * for each mbmi member value in the mi structure.
 */
static void print_mi_data(VP10_COMMON *cm, FILE *file, const char *descriptor,
                          size_t member_offset) {
  int mi_row, mi_col;
  MODE_INFO **mi = cm->mi_grid_visible;
  int rows = cm->mi_rows;
  int cols = cm->mi_cols;
  char prefix = descriptor[0];

  log_frame_info(cm, descriptor, file);
  for (mi_row = 0; mi_row < rows; mi_row++) {
    fprintf(file, "%c ", prefix);
    for (mi_col = 0; mi_col < cols; mi_col++) {
      fprintf(file, "%2d ", *((int *)((char *)(&mi[0]->mbmi) + member_offset)));
      mi++;
    }
    fprintf(file, "\n");
    mi += 8;
  }
  fprintf(file, "\n");
}

void vp10_print_modes_and_motion_vectors(VP10_COMMON *cm, const char *file) {
  int mi_row;
  int mi_col;
  FILE *mvs = fopen(file, "a");
  MODE_INFO **mi = cm->mi_grid_visible;
  int rows = cm->mi_rows;
  int cols = cm->mi_cols;

  print_mi_data(cm, mvs, "Partitions:", offsetof(MB_MODE_INFO, sb_type));
  print_mi_data(cm, mvs, "Modes:", offsetof(MB_MODE_INFO, mode));
  print_mi_data(cm, mvs, "Ref frame:", offsetof(MB_MODE_INFO, ref_frame[0]));
  print_mi_data(cm, mvs, "Transform:", offsetof(MB_MODE_INFO, tx_size));
  print_mi_data(cm, mvs, "UV Modes:", offsetof(MB_MODE_INFO, uv_mode));

  // output skip infomation.
  log_frame_info(cm, "Skips:", mvs);
  for (mi_row = 0; mi_row < rows; mi_row++) {
    fprintf(mvs, "S ");
    for (mi_col = 0; mi_col < cols; mi_col++) {
      fprintf(mvs, "%2d ", mi[0]->mbmi.skip);
      mi++;
    }
    fprintf(mvs, "\n");
    mi += 8;
  }
  fprintf(mvs, "\n");

  // output motion vectors.
  log_frame_info(cm, "Vectors ", mvs);
  mi = cm->mi_grid_visible;
  for (mi_row = 0; mi_row < rows; mi_row++) {
    fprintf(mvs, "V ");
    for (mi_col = 0; mi_col < cols; mi_col++) {
      fprintf(mvs, "%4d:%4d ", mi[0]->mbmi.mv[0].as_mv.row,
              mi[0]->mbmi.mv[0].as_mv.col);
      mi++;
    }
    fprintf(mvs, "\n");
    mi += 8;
  }
  fprintf(mvs, "\n");

  fclose(mvs);
}
