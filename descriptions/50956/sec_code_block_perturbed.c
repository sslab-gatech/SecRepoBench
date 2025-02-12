_cmsStageMatrixData* Data2 = (_cmsStageMatrixData*)cmsStageData(Matrix2);

              // Only RGB to RGB
              if (Matrix1->InputChannels != 3 || Matrix1->OutputChannels != 3 ||
                  Matrix1->InputChannels != 3 || Matrix2->OutputChannels != 3) return FALSE;

              // Input offset should be zero