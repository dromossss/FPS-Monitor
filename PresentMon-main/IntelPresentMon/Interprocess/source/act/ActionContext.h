#pragma once

namespace pmon::ipc::act
{
	template<class E>
	concept HasCustomSessionDispose = requires(E ctx, typename E::SessionContextType stx) {
		ctx.Dispose(stx);
	};
}