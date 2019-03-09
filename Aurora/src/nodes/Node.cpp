#include "Node.h"

AE_NS_BEGIN

Node::Node() :
	name(),
	_parent(nullptr),
	_root(this),
	_prev(nullptr),
	_next(nullptr),
	_childHead(nullptr),
	_numChildren(0),
	_lr(),
	_ls(Vector3::ONE),
	_lm(),
	_wr(),
	_wm(),
	_iwm(),
	_dirty(0) {
}

Node::~Node() {
}

Node* Node::addChild(Node* child) {
	if (child && !child->_parent && child != _root) {
		child->retain();
		_addNode(child);
		child->_parentChanged(_root);
		return child;
	}
	return nullptr;
}

Node* Node::insertChild(Node* child, Node* before) {
	if (child && child != _root) {
		if (child == before) return child;

		if (before) {
			if (before->_parent == this) {
				if (child->_parent == this) {
					if (child != before && child->_prev != before) {
						_removeNode(child);
						_insertNode(child, before);
					}
				} else if (!child->_parent) {
					child->retain();
					_insertNode(child, before);
				}

				return child;
			}
		} else {
			if (child->_parent == this) {
				_removeNode(child);
				_addNode(child);
			} else {
				return addChild(child);
			}
		}
	}

	return nullptr;
}

bool Node::removeChild(Node* child) {
	if (child && child->_parent == this) {
		_removeNode(child);
		child->_parentChanged(child);
		child->release();
		return true;
	}
	return false;
}

bool Node::removeFromParent() {
	return _parent ? _parent->removeChild(this) : false;
}

void Node::removeAllChildren() {
	if (_childHead) {
		for (auto& i : _traversingStack) i = nullptr;

		auto child = _childHead;
		do {
			auto next = child->_next;

			child->_prev = nullptr;
			child->_next = nullptr;
			child->_parent = nullptr;
			child->_parentChanged(child);
			child->release();

			child = next;
		} while (child);

		_childHead = nullptr;
		_numChildren = 0;
	}
}

void Node::setLocalPosition(const Vector3& p) {
	_lm.setPosition(p);

	_checkNoticeUpdate(DirtyFlag::WMIM);
}

void Node::localTranslate(const Vector3& p) {
	updateLocalMatrix();
	_lm.prependTranslate(p);

	_checkNoticeUpdate(DirtyFlag::WMIM);
}

void Node::setLocalRotation(const Quaternion& q) {
	_lr.set(q);

	_checkNoticeUpdate(DirtyFlag::LM_WRMIM, DirtyFlag::WRMIM);
}

void Node::localRotate(const Quaternion& q) {
	q.append(_lr, _lr);

	_checkNoticeUpdate(DirtyFlag::LM_WRMIM, DirtyFlag::WRMIM);
}

void Node::setLocalScale(const Vector3& s) {
	_ls.set(s);

	_checkNoticeUpdate(DirtyFlag::LM_WMIM, DirtyFlag::WMIM);
}

void Node::setLocalMatrix(const Matrix34& m) {
	_lm.set34(m);
	_localDecomposition();

	_checkNoticeUpdateNow(_dirty & DirtyFlag::NOT_LM | DirtyFlag::WRMIM, DirtyFlag::WRMIM);
}

void Node::setLocalTRS(const Vector3& pos, const Quaternion& rot, const Vector3& scale) {
	_lm.setPosition(pos);
	_lr.set(rot);
	_ls.set(scale);

	_checkNoticeUpdate(DirtyFlag::LM_WRMIM, DirtyFlag::WRMIM);
}

void Node::parentRotate(const Quaternion& q) {
	_lr.append(q);

	_checkNoticeUpdate(DirtyFlag::LM_WRMIM, DirtyFlag::WRMIM);
}

void Node::setWorldPosition(const Vector3& p) {
	auto old = _dirty;
	updateWorldMatrix();
	_wm.setPosition(p);

	_worldPositionChanged(old);
}

void Node::worldTranslate(const Vector3& p) {
	auto old = _dirty;
	updateWorldMatrix();
	_wm.prependTranslate(p);

	_worldPositionChanged(old);
}

void Node::setWorldRotation(const Quaternion& q) {
	_wr.set(q);

	_worldRotationChanged(_dirty);
}

void Node::worldRotate(const Quaternion& q) {
	auto old = _dirty;
	updateWorldRotation();
	q.append(_wr, _wr);

	_worldRotationChanged(old);
}

void Node::setWorldMatrix(const Matrix34& m) {
	_wm.set34(m);

	auto now = _dirty & DirtyFlag::NOT_WM | DirtyFlag::WIM;

	if (_parent) {
		_wm.append(getInverseWorldMatrix(), _lm);
	} else {
		_lm.set34(_wm);
	}

	_localDecomposition();

	_checkNoticeUpdateNow(now & DirtyFlag::NOT_LM | DirtyFlag::WR, DirtyFlag::WRMIM);
}

void Node::setIdentity() {
	if (!_lr.isIdentity() || !_ls.isOne() || _lm.m34[0][3] != 0.f || _lm.m34[1][3] != 0.f || _lm.m34[2][3] != 0.f) {
		_lm.set34();
		_lr.set();
		_ls.set(Vector3::ONE);

		_checkNoticeUpdate(DirtyFlag::LM_WRMIM, DirtyFlag::WRMIM);
	}
}

void Node::updateLocalMatrix() {
	if (_dirty & DirtyFlag::LM) {
		_dirty &= DirtyFlag::NOT_LM;

		_lr.toMatrix(_lm);
		_lm.prependScale(_ls);
	}
}

void Node::updateWorldRotation() {
	if (_dirty & DirtyFlag::WR) {
		_dirty &= DirtyFlag::NOT_WR;

		auto p = _parent;
		if (p) {
			p->updateWorldRotation();
			_lr.append(_wr, _wr);
		} else {
			_wr.set(_lr);
		}
	}
}

void Node::updateWorldMatrix() {
	if (_dirty & DirtyFlag::WM) {
		_dirty &= DirtyFlag::NOT_WM;

		updateLocalMatrix();
		auto p = _parent;
		if (p) {
			p->updateWorldMatrix();
			_lm.append(p->_wm, _wm);
		} else {
			_wm.set34(_lm);
		}
	}
}

void Node::updateInverseWorldMatrix() {
	if (_dirty & DirtyFlag::WIM) {
		_dirty &= DirtyFlag::NOT_WIM;

		updateWorldMatrix();
		_wm.invert(_iwm);
	}
}

void Node::getLocalRotationFromWorld(const Node& node, const Quaternion& worldRot, Quaternion& dst) {
	if (node._parent) {
		auto q = node._parent->getWorldRotation();
		q.invert();
		worldRot.append(q, dst);
	} else {
		dst.set(worldRot);
	}
}

void Node::_addNode(Node* child) {
	if (_childHead) {
		auto tail = _childHead->_prev;

		tail->_next = child;
		child->_prev = tail;
		_childHead->_prev = child;
	} else {
		_childHead = child;
		child->_prev = child;
	}

	child->_parent = this;
	++_numChildren;
}

void Node::_insertNode(Node* child, Node* before) {
	child->_next = before;
	child->_prev = before->_prev;
	if (before == _childHead) {
		_childHead = child;
	} else {
		before->_prev->_next = child;
	}
	before->_prev = child;

	child->_parent = this;
	++_numChildren;
}

void Node::_removeNode(Node* child) {
	for (auto& i : _traversingStack) {
		if (i == child) i = child->_next;
	}

	auto next = child->_next;

	if (_childHead == child) {
		_childHead = next;

		if (next) next->_prev = child->_prev;
	} else {
		auto prev = child->_prev;

		prev->_next = next;
		if (next) {
			next->_prev = prev;
		} else {
			_childHead->_prev = prev;
		}
	}

	child->_prev = nullptr;
	child->_next = nullptr;
	child->_parent = nullptr;

	--_numChildren;
}

void Node::_addChild(Node* child) {
	if (_childHead) {
		auto tail = _childHead->_prev;

		tail->_next = child;
		child->_prev = tail;
		_childHead->_prev = child;
	} else {
		_childHead = child;
		child->_prev = child;
	}

	child->_parent = this;
	++_numChildren;
}

void Node::_parentChanged(Node* root) {
	_root = root;

	_checkNoticeUpdate(DirtyFlag::WRMIM);
}

void Node::_worldPositionChanged(ui32 oldDirty) {
	auto p = _parent;
	if (p) {
		p->updateInverseWorldMatrix();

		f32 tmp[3] = { _wm.m34[0][3], _wm.m34[1][3], _wm.m34[2][3] };
		Math::matTransformPoint(p->_iwm.m34, tmp, tmp);

		_lm.setPosition(tmp);
	} else {
		_lm.setPosition(_wm);
	}

	_dirty |= DirtyFlag::WIM;
	if (oldDirty != _dirty) _noticeUpdate(DirtyFlag::WMIM);
}

void Node::_worldRotationChanged(ui32 oldDirty) {
	auto p = _parent;
	if (p) {
		p->updateWorldRotation();
		p->_wr.invert(_lr);
		_wr.append(_lr, _lr);
	} else {
		_lr.set(_wr);
	}

	_dirty &= DirtyFlag::NOT_WR;
	_dirty |= DirtyFlag::LM_WMIM;
	if (oldDirty != _dirty) _noticeUpdate(DirtyFlag::WRMIM);
}

void Node::_checkNoticeUpdateNow(ui32 nowDirty, ui32 sendDirty) {
	if (nowDirty != _dirty) {
		_dirty = nowDirty;

		_noticeUpdate(sendDirty);
	}
}

void Node::_noticeUpdate(ui32 dirty) {
	auto node = _childHead;
	while (node) {
		node->_checkNoticeUpdate(dirty);
		node = node->_next;
	}
}

AE_NS_END