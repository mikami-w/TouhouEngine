;
; Input signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; TEXCOORD                 0   xyz         0     NONE   float   xy  
; TEXCOORD                 1   xy          1     NONE   float   xy  
; TEXCOORD                 2   xy          2     NONE   float   xy  
; TEXCOORD                 3   xy          3     NONE   float   xy  
; TEXCOORD                 4   x           4     NONE   float   x   
; TEXCOORD                 5   xyzw        5     NONE   float   xyzw
; TEXCOORD                 6   xyzw        6     NONE   float   xyzw
;
;
; Output signature:
;
; Name                 Index   Mask Register SysValue  Format   Used
; -------------------- ----- ------ -------- -------- ------- ------
; SV_Position              0   xyzw        0      POS   float   xyzw
; TEXCOORD                 0   xy          1     NONE   float   xy  
; TEXCOORD                 1   xyzw        2     NONE   float   xyzw
;
; shader hash: 2e83f28cb6da4557c8565e01254b09ae
;
; Pipeline Runtime Information: 
;
;PSVRuntimeInfo:
; Vertex Shader
; OutputPositionPresent=1
; MinimumExpectedWaveLaneCount: 0
; MaximumExpectedWaveLaneCount: 4294967295
; UsesViewID: false
; SigInputElements: 7
; SigOutputElements: 3
; SigPatchConstOrPrimElements: 0
; SigInputVectors: 7
; SigOutputVectors[0]: 3
; SigOutputVectors[1]: 0
; SigOutputVectors[2]: 0
; SigOutputVectors[3]: 0
; EntryFunctionName: main
;
;
; Input signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; TEXCOORD                 0                              
; TEXCOORD                 1                              
; TEXCOORD                 2                              
; TEXCOORD                 3                              
; TEXCOORD                 4                              
; TEXCOORD                 5                              
; TEXCOORD                 6                              
;
; Output signature:
;
; Name                 Index             InterpMode DynIdx
; -------------------- ----- ---------------------- ------
; SV_Position              0          noperspective       
; TEXCOORD                 0                 linear       
; TEXCOORD                 1                 linear       
;
; Buffer Definitions:
;
; cbuffer ProjectionUB
; {
;
;   struct hostlayout.ProjectionUB
;   {
;
;       row_major float4x4 uProjection;               ; Offset:    0
;   
;   } ProjectionUB;                                   ; Offset:    0 Size:    64
;
; }
;
;
; Resource Bindings:
;
; Name                                 Type  Format         Dim      ID      HLSL Bind  Count
; ------------------------------ ---------- ------- ----------- ------- -------------- ------
; ProjectionUB                      cbuffer      NA          NA     CB0     cb0,space1     1
;
;
; ViewId state:
;
; Number of inputs: 28, outputs: 12
; Outputs dependent on ViewId: {  }
; Inputs contributing to computation of Outputs:
;   output 0 depends on inputs: { 0, 1, 8, 9, 12, 13, 16 }
;   output 1 depends on inputs: { 0, 1, 8, 9, 12, 13, 16 }
;   output 2 depends on inputs: { 0, 1, 8, 9, 12, 13, 16 }
;   output 3 depends on inputs: { 0, 1, 8, 9, 12, 13, 16 }
;   output 4 depends on inputs: { 4, 20, 22 }
;   output 5 depends on inputs: { 5, 21, 23 }
;   output 8 depends on inputs: { 24 }
;   output 9 depends on inputs: { 25 }
;   output 10 depends on inputs: { 26 }
;   output 11 depends on inputs: { 27 }
;
target datalayout = "e-m:e-p:32:32-i1:32-i8:32-i16:32-i32:32-i64:64-f16:32-f32:32-f64:64-n8:16:32:64"
target triple = "dxil-ms-dx"

%dx.types.Handle = type { i8* }
%dx.types.CBufRet.f32 = type { float, float, float, float }
%hostlayout.ProjectionUB = type { [4 x <4 x float>] }

define void @main() {
  %1 = call %dx.types.Handle @dx.op.createHandle(i32 57, i8 2, i32 0, i32 0, i1 false)  ; CreateHandle(resourceClass,rangeId,index,nonUniformIndex)
  %2 = call float @dx.op.loadInput.f32(i32 4, i32 6, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %3 = call float @dx.op.loadInput.f32(i32 4, i32 6, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %4 = call float @dx.op.loadInput.f32(i32 4, i32 6, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %5 = call float @dx.op.loadInput.f32(i32 4, i32 6, i32 0, i8 3, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %6 = call float @dx.op.loadInput.f32(i32 4, i32 5, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %7 = call float @dx.op.loadInput.f32(i32 4, i32 5, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %8 = call float @dx.op.loadInput.f32(i32 4, i32 5, i32 0, i8 2, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %9 = call float @dx.op.loadInput.f32(i32 4, i32 5, i32 0, i8 3, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %10 = call float @dx.op.loadInput.f32(i32 4, i32 4, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %11 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %12 = call float @dx.op.loadInput.f32(i32 4, i32 3, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %13 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %14 = call float @dx.op.loadInput.f32(i32 4, i32 2, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %15 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %16 = call float @dx.op.loadInput.f32(i32 4, i32 1, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %17 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 0, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %18 = call float @dx.op.loadInput.f32(i32 4, i32 0, i32 0, i8 1, i32 undef)  ; LoadInput(inputSigId,rowIndex,colIndex,gsVertexAxis)
  %19 = fmul fast float %17, %11
  %20 = fmul fast float %18, %12
  %21 = call float @dx.op.unary.f32(i32 13, float %10)  ; Sin(value)
  %22 = call float @dx.op.unary.f32(i32 12, float %10)  ; Cos(value)
  %23 = fmul fast float %22, %19
  %24 = fmul fast float %20, %21
  %25 = fsub fast float %23, %24
  %26 = fmul fast float %21, %19
  %27 = fmul fast float %22, %20
  %28 = fadd fast float %25, %13
  %29 = fadd fast float %26, %14
  %30 = fadd fast float %29, %27
  %31 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 0)  ; CBufferLoadLegacy(handle,regIndex)
  %32 = extractvalue %dx.types.CBufRet.f32 %31, 0
  %33 = extractvalue %dx.types.CBufRet.f32 %31, 1
  %34 = extractvalue %dx.types.CBufRet.f32 %31, 2
  %35 = extractvalue %dx.types.CBufRet.f32 %31, 3
  %36 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 1)  ; CBufferLoadLegacy(handle,regIndex)
  %37 = extractvalue %dx.types.CBufRet.f32 %36, 0
  %38 = extractvalue %dx.types.CBufRet.f32 %36, 1
  %39 = extractvalue %dx.types.CBufRet.f32 %36, 2
  %40 = extractvalue %dx.types.CBufRet.f32 %36, 3
  %41 = call %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32 59, %dx.types.Handle %1, i32 3)  ; CBufferLoadLegacy(handle,regIndex)
  %42 = extractvalue %dx.types.CBufRet.f32 %41, 0
  %43 = extractvalue %dx.types.CBufRet.f32 %41, 1
  %44 = extractvalue %dx.types.CBufRet.f32 %41, 2
  %45 = extractvalue %dx.types.CBufRet.f32 %41, 3
  %46 = fmul fast float %28, %32
  %47 = call float @dx.op.tertiary.f32(i32 46, float %30, float %37, float %46)  ; FMad(a,b,c)
  %48 = fadd fast float %42, %47
  %49 = fmul fast float %28, %33
  %50 = call float @dx.op.tertiary.f32(i32 46, float %30, float %38, float %49)  ; FMad(a,b,c)
  %51 = fadd fast float %50, %43
  %52 = fmul fast float %28, %34
  %53 = call float @dx.op.tertiary.f32(i32 46, float %30, float %39, float %52)  ; FMad(a,b,c)
  %54 = fadd fast float %53, %44
  %55 = fmul fast float %28, %35
  %56 = call float @dx.op.tertiary.f32(i32 46, float %30, float %40, float %55)  ; FMad(a,b,c)
  %57 = fadd fast float %56, %45
  %58 = fmul fast float %15, %8
  %59 = fmul fast float %16, %9
  %60 = fadd fast float %58, %6
  %61 = fadd fast float %59, %7
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 0, float %48)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 1, float %51)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 2, float %54)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 0, i32 0, i8 3, float %57)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 0, float %60)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 1, i32 0, i8 1, float %61)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 0, float %2)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 1, float %3)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 2, float %4)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  call void @dx.op.storeOutput.f32(i32 5, i32 2, i32 0, i8 3, float %5)  ; StoreOutput(outputSigId,rowIndex,colIndex,value)
  ret void
}

; Function Attrs: nounwind readnone
declare float @dx.op.loadInput.f32(i32, i32, i32, i8, i32) #0

; Function Attrs: nounwind
declare void @dx.op.storeOutput.f32(i32, i32, i32, i8, float) #1

; Function Attrs: nounwind readnone
declare float @dx.op.unary.f32(i32, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.CBufRet.f32 @dx.op.cbufferLoadLegacy.f32(i32, %dx.types.Handle, i32) #2

; Function Attrs: nounwind readnone
declare float @dx.op.tertiary.f32(i32, float, float, float) #0

; Function Attrs: nounwind readonly
declare %dx.types.Handle @dx.op.createHandle(i32, i8, i32, i32, i1) #2

attributes #0 = { nounwind readnone }
attributes #1 = { nounwind }
attributes #2 = { nounwind readonly }

!llvm.ident = !{!0}
!dx.version = !{!1}
!dx.valver = !{!2}
!dx.shaderModel = !{!3}
!dx.resources = !{!4}
!dx.viewIdState = !{!7}
!dx.entryPoints = !{!8}

!0 = !{!"dxcoob 1.8.2502.11 (239921522)"}
!1 = !{i32 1, i32 0}
!2 = !{i32 1, i32 8}
!3 = !{!"vs", i32 6, i32 0}
!4 = !{null, null, !5, null}
!5 = !{!6}
!6 = !{i32 0, %hostlayout.ProjectionUB* undef, !"", i32 1, i32 0, i32 1, i32 64, null}
!7 = !{[30 x i32] [i32 28, i32 12, i32 15, i32 15, i32 0, i32 0, i32 16, i32 32, i32 0, i32 0, i32 15, i32 15, i32 0, i32 0, i32 15, i32 15, i32 0, i32 0, i32 15, i32 0, i32 0, i32 0, i32 16, i32 32, i32 16, i32 32, i32 256, i32 512, i32 1024, i32 2048]}
!8 = !{void ()* @main, !"main", !9, !4, null}
!9 = !{!10, !28, null}
!10 = !{!11, !14, !16, !18, !20, !23, !26}
!11 = !{i32 0, !"TEXCOORD", i8 9, i8 0, !12, i8 0, i32 1, i8 3, i32 0, i8 0, !13}
!12 = !{i32 0}
!13 = !{i32 3, i32 3}
!14 = !{i32 1, !"TEXCOORD", i8 9, i8 0, !15, i8 0, i32 1, i8 2, i32 1, i8 0, !13}
!15 = !{i32 1}
!16 = !{i32 2, !"TEXCOORD", i8 9, i8 0, !17, i8 0, i32 1, i8 2, i32 2, i8 0, !13}
!17 = !{i32 2}
!18 = !{i32 3, !"TEXCOORD", i8 9, i8 0, !19, i8 0, i32 1, i8 2, i32 3, i8 0, !13}
!19 = !{i32 3}
!20 = !{i32 4, !"TEXCOORD", i8 9, i8 0, !21, i8 0, i32 1, i8 1, i32 4, i8 0, !22}
!21 = !{i32 4}
!22 = !{i32 3, i32 1}
!23 = !{i32 5, !"TEXCOORD", i8 9, i8 0, !24, i8 0, i32 1, i8 4, i32 5, i8 0, !25}
!24 = !{i32 5}
!25 = !{i32 3, i32 15}
!26 = !{i32 6, !"TEXCOORD", i8 9, i8 0, !27, i8 0, i32 1, i8 4, i32 6, i8 0, !25}
!27 = !{i32 6}
!28 = !{!29, !30, !31}
!29 = !{i32 0, !"SV_Position", i8 9, i8 3, !12, i8 4, i32 1, i8 4, i32 0, i8 0, !25}
!30 = !{i32 1, !"TEXCOORD", i8 9, i8 0, !12, i8 2, i32 1, i8 2, i32 1, i8 0, !13}
!31 = !{i32 2, !"TEXCOORD", i8 9, i8 0, !15, i8 2, i32 1, i8 4, i32 2, i8 0, !25}
