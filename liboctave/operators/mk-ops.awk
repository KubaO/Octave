# Copyright (C) 2003-2015 John W. Eaton
#
# This file is part of Octave.
#
# Octave is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 3 of the License, or (at
# your option) any later version.
#
# Octave is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License
# along with Octave; see the file COPYING.  If not, see
# <http://www.gnu.org/licenses/>.

BEGIN {
  declare_types = 0;
  generate_ops = 0;
  op_class = "";
  sparse = 0;
  ntypes = 0;
  make_inclusive_header = 0;
  make_header = 0;
  make_source = 0;

  if (build_file)
    {
      if (build_file == "mx-ops.h" || build_file == "smx-ops.h" || build_file == "vx-ops.h")
        {
          make_inclusive_header = 1;

          print "// DO NOT EDIT -- generated by mk-ops.awk";
          tmp = build_file;
          prefix = substr (tmp, 1, index (tmp, "-")-1);
          gsub (/[\.-]/, "_", tmp);
          printf ("#if ! defined (octave_%s)\n", tmp);
          printf ("#define octave_%s 1\n", tmp);
          print "#include \"octave-config.h\"";
        }
      else
        {
          if (match (build_file, /\.h$/))
            {
              make_header = 1;

              tmp = substr (build_file, 1, length (build_file)-2);
            }
          else if (match (build_file, /\.cc$/))
            {
              make_source = 1;

              tmp = substr (build_file, 1, length (build_file)-3);
            }

          prefix = substr (tmp, 1, index (tmp, "-")-1);
          tmp = substr (tmp, index (tmp, "-")+1);
          lhs_arg = substr (tmp, 1, index (tmp, "-")-1);
          rhs_arg = substr (tmp, index (tmp, "-")+1);
        }
    }
}
/^#/ {
  if ($2 == "types")
    declare_types = 1;
  else if ($3 == "operators")
    {
      op_class = $2;
      generate_ops = 1;
      declare_types = 0;

      if (op_class == "vector")
        sparse = 0;
      else if (op_class == "full-matrix")
        sparse = 0;
      else if (op_class == "sparse-matrix")
        sparse = 1;
      else
        {
          printf ("unrecognized object type: %s", op_class);
          exit (1);
        }
    }
  next;
} {
  if (declare_types)
    {
      ntypes++;

      if (NF == 6 || NF == 7)
        {
          if (NF == 7)
            core_type[ntypes] = $7;

          scalar_zero_val[ntypes] = $6;
          fwd_decl_ok[ntypes] = $5 == "YES";
          header[ntypes] = $4 == "NONE" ? "" : $4;
          class[ntypes] = $3;
          type[ntypes] = $2;
          tag[ntypes] = $1;
          rev_tag[$1] = ntypes;
        }
      else
        printf ("skipping line %d: %s\n", NR, $0);
    }
  else if (generate_ops)
    {
      if (NF >= 4 || (sparse && NF >= 5))
        {
          n = 0;

          result_tag_1 = $(++n);
          if (sparse)
            result_tag_2 = $(++n);
          lhs_tag = $(++n);
          rhs_tag = $(++n);
          op_type = $(++n);

          bin_ops = index (op_type, "B") != 0;
          cmp_ops = index (op_type, "C") != 0;
          eqne_ops = index (op_type, "E") != 0;
          bool_ops = index (op_type, "L") != 0;

          if (cmp_ops)
            {
              lhs_conv = $(++n);
              rhs_conv = $(++n);
            }

          if (lhs_conv == "NONE")
            lhs_conv = "";

          if (rhs_conv == "NONE")
            rhs_conv = "";

          k = 0
          while (NF > n)
            bool_headers[k++] = $(++n);

          cc_file = sprintf ("%s-%s-%s.cc", prefix, lhs_tag, rhs_tag);
          h_file = sprintf ("%s-%s-%s.h", prefix, lhs_tag, rhs_tag);

          if (list_cc_files)
            {
              printf (" liboctave/operators/%s", cc_file);
              next;
            }

          if (list_h_files)
            {
              printf (" liboctave/operators/%s", h_file);
              next;
            }

          if (make_inclusive_header)
            {
              printf ("#include \"%s\"\n", h_file);
              next;
            }

          if (lhs_arg != lhs_tag || rhs_arg != rhs_tag)
            next;

          h_guard = sprintf ("octave_%s_%s_%s_h", prefix, lhs_tag, rhs_tag);

          result_num_1 = rev_tag[result_tag_1];
          if (sparse)
            result_num_2 = rev_tag[result_tag_2];
          lhs_num = rev_tag[lhs_tag];
          rhs_num = rev_tag[rhs_tag];

          result_type_1 = type[result_num_1];
          if (sparse)
            result_type_2 = type[result_num_2];
          lhs_type = type[lhs_num];
          rhs_type = type[rhs_num];

          lhs_core_type = core_type[lhs_num];
          rhs_core_type = core_type[rhs_num];

          result_scalar_zero_val_1 = scalar_zero_val[result_num_1];
          if (sparse)
            result_scalar_zero_val_2 = scalar_zero_val[result_num_2];
          lhs_scalar_zero_val = scalar_zero_val[lhs_num];
          rhs_scalar_zero_val = scalar_zero_val[rhs_num];

          result_header_1 = header[result_num_1];
          if (sparse)
            result_header_2 = header[result_num_2];
          lhs_header = header[lhs_num];
          rhs_header = header[rhs_num];

          lhs_class = class[lhs_num];
          rhs_class = class[rhs_num];

          if (make_header)
            {
              print "// DO NOT EDIT -- generated by mk-ops.awk";

              printf ("#if ! defined (%s)\n", h_guard);
              printf ("#define %s 1\n", h_guard);
              print "#include \"octave-config.h\"";

              if (result_header_1)
                {
                  if (result_fwd_decl_ok)
                    printf ("class %s\n", result_type_1);
                  else
                    printf ("#include \"%s\"\n", result_header_1);
                }

              if (result_header_2 && ! (result_header_2 == result_header_1))
                {
                  if (result_fwd_decl_ok)
                    printf ("class %s\n", result_type_2);
                  else
                    printf ("#include \"%s\"\n", result_header_2);
                }

              if (lhs_header && ! (lhs_header == result_header_1 || lhs_header == result_header_2))
                {
                  if (result_fwd_decl_ok)
                    printf ("class %s\n", lhs_type);
                  else
                    printf ("#include \"%s\"\n", lhs_header);
                }

              if (rhs_header && ! (rhs_header == lhs_header || rhs_header == result_header_1 || rhs_header == result_header_2))
                {
                  if (result_fwd_decl_ok)
                    printf ("class %s\n", rhs_type);
                  else
                    printf ("#include \"%s\"\n", rhs_header);
                }

              if (sparse)
                {
                  ## FIXME: it might be nice to only include the declarations
                  ## of the operators that are actually needed instead of
                  ## including all of them.
                  printf ("#include \"mx-ops.h\"\n");
                  printf ("#include \"Sparse-op-defs.h\"\n");
                }
              else
                printf ("#include \"mx-op-decl.h\"\n");

              if (bin_ops)
                {
                  if (sparse)
                    printf ("SPARSE_%s%s_BIN_OP_DECLS (%s, %s, %s, %s, OCTAVE_API)\n",
                            lhs_class, rhs_class, result_type_1, result_type_2,
                            lhs_type, rhs_type);
                  else
                    printf ("%s%s_BIN_OP_DECLS (%s, %s, %s, OCTAVE_API)\n",
                            lhs_class, rhs_class, result_type_1, lhs_type,
                            rhs_type);
                }

              if (cmp_ops)
                {
                  if (sparse)
                    printf ("SPARSE_%s%s_CMP_OP_DECLS (%s, %s, OCTAVE_API)\n",
                            lhs_class, rhs_class, lhs_type, rhs_type);
                  else
                    printf ("%s%s_CMP_OP_DECLS (%s, %s, OCTAVE_API)\n",
                            lhs_class, rhs_class, lhs_type, rhs_type);
                }

              if (eqne_ops)
                {
                  if (sparse)
                    printf ("SPARSE_%s%s_EQNE_OP_DECLS (%s, %s, OCTAVE_API)\n",
                            lhs_class, rhs_class, lhs_type, rhs_type);
                }

              if (bool_ops)
                {
                  if (sparse)
                    printf ("SPARSE_%s%s_BOOL_OP_DECLS (%s, %s, OCTAVE_API)\n",
                            lhs_class, rhs_class, lhs_type, rhs_type);
                  else
                    printf ("%s%s_BOOL_OP_DECLS (%s, %s, OCTAVE_API)\n",
                            lhs_class, rhs_class, lhs_type, rhs_type);
                }

              print "#endif";

              exit (0);
            }
          else if (make_source)
            {
              print "// DO NOT EDIT -- generated by mk-ops.awk";

              print "#if defined (HAVE_CONFIG_H)";
              print "#  include \"config.h\"";
              print "#endif";

              print "#include \"Array-util.h\"";

              printf ("#include \"%s\"\n", h_file);

              if (! sparse)
                printf ("#include \"mx-op-defs.h\"\n");

              for (i in bool_headers)
                {
                  printf ("#include \"%s\"\n", bool_headers[i]);
                  delete bool_headers[i];
                }

              if (result_header_1)
                printf ("#include \"%s\"\n", result_header_1);

              if (result_header_2 && ! (result_header_2 == result_header_1))
                printf ("#include \"%s\"\n", result_header_2);

              if (lhs_header && ! (lhs_header == result_header_1 || lhs_header == result_header_2))
                printf ("#include \"%s\"\n", lhs_header);

              if (rhs_header && ! (rhs_header == lhs_header || rhs_header == result_header_1 || rhs_heaer == result_header_2))
                printf ("#include \"%s\"\n", rhs_header);

              if (bin_ops)
                {
                  if (sparse)
                    printf ("SPARSE_%s%s_BIN_OPS (%s, %s, %s, %s)\n",
                            lhs_class, rhs_class, result_type_1,
                            result_type_2, lhs_type, rhs_type);
                  else
                    {
                      if ((lhs_class == "DM" && rhs_class == "M") || (lhs_class == "M" && rhs_class == "DM"))
                        printf ("%s%s_BIN_OPS (%s, %s, %s, %s)\n",
                                lhs_class, rhs_class, result_type_1,
                                lhs_type, rhs_type, result_scalar_zero_val_1);
                      else
                        printf ("%s%s_BIN_OPS (%s, %s, %s)\n",
                                lhs_class, rhs_class, result_type_1,
                                lhs_type, rhs_type);
                    }
                }

              if (cmp_ops)
                {
                  if (sparse)
                    printf ("SPARSE_%s%s_CMP_OPS (%s, %s, %s, %s, %s, %s)\n",
                            lhs_class, rhs_class, lhs_type,
                            lhs_scalar_zero_val, lhs_conv, rhs_type,
                            rhs_scalar_zero_val, rhs_conv);
                  else
                    printf ("%s%s_CMP_OPS (%s, %s)\n",
                            lhs_class, rhs_class, lhs_type, rhs_type);
                }

              if (eqne_ops)
                {
                  if (sparse)
                    printf ("SPARSE_%s%s_EQNE_OPS (%s, %s, %s, %s, %s, %s)\n",
                            lhs_class, rhs_class, lhs_type,
                            lhs_scalar_zero_val, lhs_conv, rhs_type,
                            rhs_scalar_zero_val, rhs_conv);

                  ## No separate eqne ops for full-matrix or vector.
                }

              if (bool_ops)
                {
                  if (sparse)
                    printf ("SPARSE_%s%s_BOOL_OPS2 (%s, %s, %s, %s)\n",
                            lhs_class, rhs_class, lhs_type, rhs_type,
                            lhs_scalar_zero_val, rhs_scalar_zero_val);
                  else
                    printf ("%s%s_BOOL_OPS (%s, %s)\n", lhs_class, rhs_class,
                            lhs_type, rhs_type);
                }

              exit (0);
            }
        }
      else
        printf ("skipping line %d: %s\n", NR, $0);
    }
}
END {
  if (make_inclusive_header)
    print "#endif";
}
